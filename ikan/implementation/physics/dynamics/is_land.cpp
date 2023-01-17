//
//  is_land.cpp
//  ikan
//
//  Created by Ashish . on 16/01/23.
//

#include "is_land.hpp"

#include "body.hpp"
#include "contact.hpp"
#include "distance.hpp"
#include "fixture.hpp"
#include "joint.hpp"
#include "stack_allocator.hpp"
#include "timer.hpp"
#include "world.hpp"

#include "contact_solver.hpp"

namespace physics {
  
  /*
   Position Correction Notes
   =========================
   I tried the several algorithms for position correction of the 2D revolute joint.
   I looked at these systems:
   - simple pendulum (1m diameter sphere on massless 5m stick) with initial angular velocity of 100 rad/s.
   - suspension bridge with 30 1m long planks of length 1m.
   - multi-link chain with 30 1m long links.
   
   Here are the algorithms:
   
   Baumgarte - A fraction of the position error is added to the velocity error. There is no
   separate position solver.
   
   Pseudo Velocities - After the velocity solver and position integration,
   the position error, Jacobian, and effective mass are recomputed. Then
   the velocity constraints are solved with pseudo velocities and a fraction
   of the position error is added to the pseudo velocity error. The pseudo
   velocities are initialized to zero and there is no warm-starting. After
   the position solver, the pseudo velocities are added to the positions.
   This is also called the First Order World method or the Position LCP method.
   
   Modified Nonlinear Gauss-Seidel (NGS) - Like Pseudo Velocities except the
   position error is re-computed for each constraint and the positions are updated
   after the constraint is solved. The radius vectors (aka Jacobians) are
   re-computed too (otherwise the algorithm has horrible instability). The pseudo
   velocity states are not needed because they are effectively zero at the beginning
   of each iteration. Since we have the current position error, we allow the
   iterations to terminate early if the error becomes smaller than _linearSlop.
   
   Full NGS or just NGS - Like Modified NGS except the effective mass are re-computed
   each time a constraint is solved.
   
   Here are the results:
   Baumgarte - this is the cheapest algorithm but it has some stability problems,
   especially with the bridge. The chain links separate easily close to the root
   and they jitter as they struggle to pull together. This is one of the most common
   methods in the field. The big drawback is that the position correction artificially
   affects the momentum, thus leading to instabilities and false bounce. I used a
   bias factor of 0.2. A larger bias factor makes the bridge less stable, a smaller
   factor makes joints and contacts more spongy.
   
   Pseudo Velocities - the is more stable than the Baumgarte method. The bridge is
   stable. However, joints still separate with large angular velocities. Drag the
   simple pendulum in a circle quickly and the joint will separate. The chain separates
   easily and does not recover. I used a bias factor of 0.2. A larger value lead to
   the bridge collapsing when a heavy cube drops on it.
   
   Modified NGS - this algorithm is better in some ways than Baumgarte and Pseudo
   Velocities, but in other ways it is worse. The bridge and chain are much more
   stable, but the simple pendulum goes unstable at high angular velocities.
   
   Full NGS - stable in all tests. The joints display good stiffness. The bridge
   still sags, but this is better than infinite forces.
   
   Recommendations
   Pseudo Velocities are not really worthwhile because the bridge and chain cannot
   recover from joint separation. In other cases the benefit over Baumgarte is small.
   
   Modified NGS is not a robust method for the revolute joint due to the violent
   instability seen in the simple pendulum. Perhaps it is viable with other constraint
   types, especially scalar constraints where the effective mass is a scalar.
   
   This leaves Baumgarte and Full NGS. Baumgarte has small, but manageable instabilities
   and is very fast. I don't think we can escape Baumgarte, especially in highly
   demanding cases where high constraint fidelity is not needed.
   
   Full NGS is robust and easy on the eyes. I recommend this as an option for
   higher fidelity simulation and certainly for suspension bridges and long chains.
   Full NGS might be a good choice for ragdolls, especially motorized ragdolls where
   joint separation can be problematic. The number of NGS iterations can be reduced
   for better performance without harming robustness much.
   
   Each joint in a can be handled differently in the position solver. So I recommend
   a system where the user can select the algorithm on a per joint basis. I would
   probably default to the slower Full NGS and let the user select the faster
   Baumgarte method in performance critical scenarios.
   */
  
  /*
   Cache Performance
   
   The Box2D solvers are dominated by cache misses. Data structures are designed
   to increase the number of cache hits. Much of misses are due to random access
   to body data. The constraint structures are iterated over linearly, which leads
   to few cache misses.
   
   The bodies are not accessed during iteration. Instead read only data, such as
   the mass values are stored with the constraints. The mutable data are the constraint
   impulses and the bodies velocities/positions. The impulses are held inside the
   constraint structures. The body velocities/positions are held in compact, temporary
   arrays to increase the number of cache hits. Linear and angular velocity are
   stored in a single array since multiple arrays lead to multiple misses.
   */
  
  /*
   2D Rotation
   
   R = [cos(theta) -sin(theta)]
   [sin(theta) cos(theta) ]
   
   thetaDot = omega
   
   Let q1 = cos(theta), q2 = sin(theta).
   R = [q1 -q2]
   [q2  q1]
   
   q1Dot = -thetaDot * q2
   q2Dot = thetaDot * q1
   
   q1_new = q1_old - dt * w * q2
   q2_new = q2_old + dt * w * q1
   then normalize.
   
   This might be faster than computing sin+cos.
   However, we can compute sin+cos of the same angle fast.
   */
  
  Island::Island(int32_t bodyCapacity,
                 int32_t contactCapacity,
                 int32_t jointCapacity,
                 StackAllocator* allocator,
                 ContactListener* listener) {
    body_capacity_ = bodyCapacity;
    contact_capacity_ = contactCapacity;
    joint_capacity_   = jointCapacity;
    body_count_ = 0;
    contact_count_ = 0;
    join_count_ = 0;
    
    allocator_ = allocator;
    listner_ = listener;
    
    bodies_ = (Body**)allocator->Allocate(bodyCapacity * sizeof(Body*));
    contacts_ = (Contact**)allocator->Allocate(contactCapacity   * sizeof(Contact*));
    joints_ = (Joint**)allocator->Allocate(jointCapacity * sizeof(Joint*));
    
    velocities_ = (Velocity*)allocator->Allocate(body_capacity_ * sizeof(Velocity));
    positions_ = (Position*)allocator->Allocate(body_capacity_ * sizeof(Position));
  }
  
  Island::~Island() {
    // Warning: the order should reverse the constructor order.
    allocator_->Free(positions_);
    allocator_->Free(velocities_);
    allocator_->Free(joints_);
    allocator_->Free(contacts_);
    allocator_->Free(bodies_);
  }
  
  void Island::Solve(Profile* profile, const TimeStep& step, const Vec2& gravity, bool allowSleep) {
    Timer timer;
    
    float h = step.dt;
    
    // Integrate velocities and apply damping. Initialize the body state.
    for (int32_t i = 0; i < body_count_; ++i) {
      Body* b = bodies_[i];
      
      Vec2 c = b->sweep_.c;
      float a = b->sweep_.a;
      Vec2 v = b->linear_velocity_;
      float w = b->angular_velocity_;
      
      // Store positions for continuous collision.
      b->sweep_.c0 = b->sweep_.c;
      b->sweep_.a0 = b->sweep_.a;
      
      if (b->type_ == BodyType::DynamicBody) {
        // Integrate velocities.
        v += h * b->inv_mass_ * (b->gravity_scale_ * b->mass_ * gravity + b->force_);
        w += h * b->inv_inertia_ * b->torque_;
        
        // Apply damping.
        // ODE: dv/dt + c * v = 0
        // Solution: v(t) = v0 * exp(-c * t)
        // Time step: v(t + dt) = v0 * exp(-c * (t + dt)) = v0 * exp(-c * t) * exp(-c * dt) = v * exp(-c * dt)
        // v2 = exp(-c * dt) * v1
        // Pade approximation:
        // v2 = v1 * 1 / (1 + c * dt)
        v *= 1.0f / (1.0f + h * b->linear_damping_);
        w *= 1.0f / (1.0f + h * b->angular_damping_);
      }
      
      positions_[i].c = c;
      positions_[i].a = a;
      velocities_[i].v = v;
      velocities_[i].w = w;
    }
    
    timer.Reset();
    
    // Solver data
    SolverData solverData;
    solverData.step = step;
    solverData.positions = positions_;
    solverData.velocities = velocities_;
    
    // Initialize velocity constraints.
    ContactSolverDef contactSolverDef;
    contactSolverDef.step = step;
    contactSolverDef.contacts = contacts_;
    contactSolverDef.count = contact_count_;
    contactSolverDef.positions = positions_;
    contactSolverDef.velocities = velocities_;
    contactSolverDef.allocator = allocator_;
    
    ContactSolver contactSolver(&contactSolverDef);
    contactSolver.InitializeVelocityConstraints();
    
    if (step.warm_starting) {
      contactSolver.WarmStart();
    }
    
    for (int32_t i = 0; i < join_count_; ++i) {
      joints_[i]->InitVelocityConstraints(solverData);
    }
    
    profile->solveInit = timer.GetMilliseconds();
    
    // Solve velocity constraints
    timer.Reset();
    for (int32_t i = 0; i < step.velocity_iterations; ++i) {
      for (int32_t j = 0; j < join_count_; ++j) {
        joints_[j]->SolveVelocityConstraints(solverData);
      }
      
      contactSolver.SolveVelocityConstraints();
    }
    
    // Store impulses for warm starting
    contactSolver.StoreImpulses();
    profile->solveVelocity = timer.GetMilliseconds();
    
    // Integrate positions
    for (int32_t i = 0; i < body_count_; ++i) {
      Vec2 c = positions_[i].c;
      float a = positions_[i].a;
      Vec2 v = velocities_[i].v;
      float w = velocities_[i].w;
      
      // Check for large velocities
      Vec2 translation = h * v;
      if (Dot(translation, translation) > MaxTranslationSquared) {
        float ratio = MaxTranslation / translation.Length();
        v *= ratio;
      }
      
      float rotation = h * w;
      if (rotation * rotation > MaxRotationSquared) {
        float ratio = MaxRotation / Abs(rotation);
        w *= ratio;
      }
      
      // Integrate
      c += h * v;
      a += h * w;
      
      positions_[i].c = c;
      positions_[i].a = a;
      velocities_[i].v = v;
      velocities_[i].w = w;
    }
    
    // Solve position constraints
    timer.Reset();
    bool positionSolved = false;
    for (int32_t i = 0; i < step.position_iterations; ++i) {
      bool contactsOkay = contactSolver.SolvePositionConstraints();
      
      bool jointsOkay = true;
      for (int32_t j = 0; j < join_count_; ++j) {
        bool jointOkay = joints_[j]->SolvePositionConstraints(solverData);
        jointsOkay = jointsOkay && jointOkay;
      }
      
      if (contactsOkay && jointsOkay) {
        // Exit early if the position errors are small.
        positionSolved = true;
        break;
      }
    }
    
    // Copy state buffers back to the bodies
    for (int32_t i = 0; i < body_count_; ++i) {
      Body* body = bodies_[i];
      body->sweep_.c = positions_[i].c;
      body->sweep_.a = positions_[i].a;
      body->linear_velocity_ = velocities_[i].v;
      body->angular_velocity_ = velocities_[i].w;
      body->SynchronizeTransform();
    }
    
    profile->solvePosition = timer.GetMilliseconds();
    
    Report(contactSolver.velocity_constraints_);
    
    if (allowSleep) {
      float minSleepTime = FLT_MAX;
      
      const float linTolSqr = LinearSleepTolerance * LinearSleepTolerance;
      const float angTolSqr = AngularSleepTolerance * AngularSleepTolerance;
      
      for (int32_t i = 0; i < body_count_; ++i) {
        Body* b = bodies_[i];
        if (b->GetType() == BodyType::StaticBody) {
          continue;
        }
        
        if ((b->flags_ & Body::e_autoSleepFlag) == 0 or
            b->angular_velocity_ * b->angular_velocity_ > angTolSqr or
            Dot(b->linear_velocity_, b->linear_velocity_) > linTolSqr) {
          b->sleep_time_ = 0.0f;
          minSleepTime = 0.0f;
        }
        else {
          b->sleep_time_ += h;
          minSleepTime = Min(minSleepTime, b->sleep_time_);
        }
      }
      
      if (minSleepTime >= TimeToSleep and positionSolved) {
        for (int32_t i = 0; i < body_count_; ++i) {
          Body* b = bodies_[i];
          b->SetAwake(false);
        }
      }
    }
  }
  
  void Island::SolveTOI(const TimeStep& subStep, int32_t toiIndexA, int32_t toiIndexB) {
    IK_ASSERT(toiIndexA < body_count_);
    IK_ASSERT(toiIndexB < body_count_);
    
    // Initialize the body state.
    for (int32_t i = 0; i < body_count_; ++i) {
      Body* b = bodies_[i];
      positions_[i].c = b->sweep_.c;
      positions_[i].a = b->sweep_.a;
      velocities_[i].v = b->linear_velocity_;
      velocities_[i].w = b->angular_velocity_;
    }
    
    ContactSolverDef contactSolverDef;
    contactSolverDef.contacts = contacts_;
    contactSolverDef.count = contact_count_;
    contactSolverDef.allocator = allocator_;
    contactSolverDef.step = subStep;
    contactSolverDef.positions = positions_;
    contactSolverDef.velocities = velocities_;
    ContactSolver contactSolver(&contactSolverDef);
    
    // Solve position constraints.
    for (int32_t i = 0; i < subStep.position_iterations; ++i) {
      bool contactsOkay = contactSolver.SolveTOIPositionConstraints(toiIndexA, toiIndexB);
      if (contactsOkay) {
        break;
      }
    }
    
    // Leap of faith to new safe state.
    bodies_[toiIndexA]->sweep_.c0 = positions_[toiIndexA].c;
    bodies_[toiIndexA]->sweep_.a0 = positions_[toiIndexA].a;
    bodies_[toiIndexB]->sweep_.c0 = positions_[toiIndexB].c;
    bodies_[toiIndexB]->sweep_.a0 = positions_[toiIndexB].a;
    
    // No warm starting is needed for TOI events because warm
    // starting impulses were applied in the discrete solver.
    contactSolver.InitializeVelocityConstraints();
    
    // Solve velocity constraints.
    for (int32_t i = 0; i < subStep.velocity_iterations; ++i) {
      contactSolver.SolveVelocityConstraints();
    }
    
    // Don't store the TOI contact forces for warm starting
    // because they can be quite large.
    
    float h = subStep.dt;
    
    // Integrate positions
    for (int32_t i = 0; i < body_count_; ++i) {
      Vec2 c = positions_[i].c;
      float a = positions_[i].a;
      Vec2 v = velocities_[i].v;
      float w = velocities_[i].w;
      
      // Check for large velocities
      Vec2 translation = h * v;
      if (Dot(translation, translation) > MaxTranslationSquared) {
        float ratio = MaxTranslation / translation.Length();
        v *= ratio;
      }
      
      float rotation = h * w;
      if (rotation * rotation > MaxRotationSquared) {
        float ratio = MaxRotation / Abs(rotation);
        w *= ratio;
      }
      
      // Integrate
      c += h * v;
      a += h * w;
      
      positions_[i].c = c;
      positions_[i].a = a;
      velocities_[i].v = v;
      velocities_[i].w = w;
      
      // Sync bodies
      Body* body = bodies_[i];
      body->sweep_.c = c;
      body->sweep_.a = a;
      body->linear_velocity_ = v;
      body->angular_velocity_ = w;
      body->SynchronizeTransform();
    }
    
    Report(contactSolver.velocity_constraints_);
  }
  
  void Island::Report(const ContactVelocityConstraint* constraints) {
    if (listner_ == nullptr) {
      return;
    }
    
    for (int32_t i = 0; i < contact_count_; ++i) {
      Contact* c = contacts_[i];
      
      const ContactVelocityConstraint* vc = constraints + i;
      
      ContactImpulse impulse;
      impulse.count = vc->pointCount;
      for (int32_t j = 0; j < vc->pointCount; ++j) {
        impulse.normal_mpulses[j] = vc->points[j].normalImpulse;
        impulse.tangent_mpulses[j] = vc->points[j].tangentImpulse;
      }
      
      listner_->PostSolve(c, &impulse);
    }
  }

  
}
