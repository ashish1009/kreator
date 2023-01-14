//
//  body.cpp
//  ikan
//
//  Created by Ashish . on 14/01/23.
//

#include "body.hpp"
#include "contact.hpp"
#include "joint.hpp"
#include "world.hpp"
#include "block_allocator.hpp"

namespace physics {
  
  Body::Body(const BodyDef* bd, World* world) {
    IK_ASSERT(bd->position.IsValid());
    IK_ASSERT(bd->linear_velocity.IsValid());
    IK_ASSERT(IsValid(bd->angle));
    IK_ASSERT(IsValid(bd->angular_velocity));
    IK_ASSERT(IsValid(bd->angular_damping) && bd->angular_damping >= 0.0f);
    IK_ASSERT(IsValid(bd->linear_damping) && bd->linear_damping >= 0.0f);
    
    flags_ = 0;
    
    if (bd->bullet) {
      flags_ |= e_bulletFlag;
    }
    if (bd->fixed_otation) {
      flags_ |= e_fixedRotationFlag;
    }
    if (bd->allow_sleep) {
      flags_ |= e_autoSleepFlag;
    }
    if (bd->awake && bd->type != BodyType::StaticBody) {
      flags_ |= e_awakeFlag;
    }
    if (bd->enabled) {
      flags_ |= e_enabledFlag;
    }
    
    world_ = world;
    
    xf_.p = bd->position;
    xf_.q.Set(bd->angle);
    
    sweep_.localCenter.SetZero();
    sweep_.c0 = xf_.p;
    sweep_.c = xf_.p;
    sweep_.a0 = bd->angle;
    sweep_.a = bd->angle;
    sweep_.alpha0 = 0.0f;
    
    joint_list_ = nullptr;
    contact_list_ = nullptr;
    prev_ = nullptr;
    next_ = nullptr;
    
    linear_velocity_ = bd->linear_velocity;
    angular_velocity_ = bd->angular_velocity;
    
    linear_damping_ = bd->linear_damping;
    angular_damping_ = bd->angular_damping;
    gravity_scale_ = bd->gravity_scale;
    
    force_.SetZero();
    torque_ = 0.0f;
    
    sleep_time_ = 0.0f;
    
    type_ = bd->type;
    
    mass_ = 0.0f;
    inv_mass_ = 0.0f;
    
    inertia_ = 0.0f;
    inv_inertia_ = 0.0f;
    
    user_data_ = bd->user_data;
    
    fixture_list_ = nullptr;
    fixture_count_ = 0;
  }
  
  Body::~Body() {
    // shapes and joints are destroyed in World::Destroy
  }
  
  void Body::SetType(BodyType type) {
    IK_ASSERT(world_->IsLocked() == false);
    if (world_->IsLocked() == true) {
      return;
    }
    
    if (type_ == type) {
      return;
    }
    
    type_ = type;
    
    ResetMassData();
    
    if (type_ == BodyType::StaticBody) {
      linear_velocity_.SetZero();
      angular_velocity_ = 0.0f;
      sweep_.a0 = sweep_.a;
      sweep_.c0 = sweep_.c;
      flags_ &= ~e_awakeFlag;
      SynchronizeFixtures();
    }
    
    SetAwake(true);
    
    force_.SetZero();
    torque_ = 0.0f;
    
    // Delete the attached contacts.
    ContactEdge* ce = contact_list_;
    while (ce) {
      ContactEdge* ce0 = ce;
      ce = ce->next;
      world_->contact_manager.Destroy(ce0->contact);
    }
    contact_list_ = nullptr;
    
    // Touch the proxies so that new contacts will be created (when appropriate)
    BroadPhase* broad_phase = &world_->contact_manager.broad_phase_;
    for (Fixture* f = fixture_list_; f; f = f->next_) {
       int32_t proxy_count = f->proxy_count_;
      for (int32_t i = 0; i < proxy_count; ++i) {
        broad_phase->TouchProxy(f->proxies_[i].proxy_id);
      }
    }
  }
  
  Fixture* Body::CreateFixture(const FixtureDef* def) {
    IK_ASSERT(world_->IsLocked() == false);
    if (world_->IsLocked() == true) {
      return nullptr;
    }
    
    BlockAllocator* allocator = &world_->m_blockAllocator;
    
    void* memory = allocator->Allocate(sizeof(Fixture));
    Fixture* fixture = new (memory) Fixture;
    fixture->Create(allocator, this, def);
    
    if (flags_ & e_enabledFlag) {
      BroadPhase* broadPhase = &world_->contact_manager.broad_phase_;
      fixture->CreateProxies(broadPhase, xf_);
    }
    
    fixture->next_ = fixture_list_;
    fixture_list_ = fixture;
    ++fixture_count_;
    
    fixture->body_ = this;
    
    // Adjust mass properties if needed.
    if (fixture->density_ > 0.0f) {
      ResetMassData();
    }
    
    // Let the world know we have a new fixture. This will cause new contacts
    // to be created at the beginning of the next time step.
    world_->m_newContacts = true;
    
    return fixture;
  }
  
  Fixture* Body::CreateFixture(const Shape* shape, float density) {
    FixtureDef def;
    def.shape = shape;
    def.density = density;
    
    return CreateFixture(&def);
  }
  
  void Body::DestroyFixture(Fixture* fixture) {
    if (fixture == NULL) {
      return;
    }
    
    IK_ASSERT(world_->IsLocked() == false);
    if (world_->IsLocked() == true) {
      return;
    }
    
    IK_ASSERT(fixture->body_ == this);
    
    // Remove the fixture from this body's singly linked list.
    IK_ASSERT(fixture_count_ > 0);
    Fixture** node = &fixture_list_;
    bool found = false;
    while (*node != nullptr) {
      if (*node == fixture) {
        *node = fixture->next_;
        found = true;
        break;
      }
      
      node = &(*node)->next_;
    }
    
    // You tried to remove a shape that is not attached to this body.
    IK_ASSERT(found);
    
    // Destroy any contacts associated with the fixture.
    ContactEdge* edge = contact_list_;
    while (edge) {
      Contact* c = edge->contact;
      edge = edge->next;
      
      Fixture* fixtureA = c->GetFixtureA();
      Fixture* fixtureB = c->GetFixtureB();
      
      if (fixture == fixtureA || fixture == fixtureB)
      {
        // This destroys the contact and removes it from
        // this body's contact list.
        world_->contact_manager.Destroy(c);
      }
    }
    
    BlockAllocator* allocator = &world_->m_blockAllocator;
    
    if (flags_ & e_enabledFlag) {
      BroadPhase* broadPhase = &world_->contact_manager.broad_phase_;
      fixture->DestroyProxies(broadPhase);
    }
    
    fixture->body_ = nullptr;
    fixture->next_ = nullptr;
    fixture->Destroy(allocator);
    fixture->~Fixture();
    allocator->Free(fixture, sizeof(Fixture));
    
    --fixture_count_;
    
    // Reset the mass data.
    ResetMassData();
  }
  
  void Body::ResetMassData() {
    // Compute mass data from shapes. Each shape has its own density.
    mass_ = 0.0f;
    inv_mass_ = 0.0f;
    inertia_ = 0.0f;
    inv_inertia_ = 0.0f;
    sweep_.localCenter.SetZero();
    
    // Static and kinematic bodies have zero mass.
    if (type_ == BodyType::StaticBody || type_ == BodyType::KinematicBody) {
      sweep_.c0 = xf_.p;
      sweep_.c = xf_.p;
      sweep_.a0 = sweep_.a;
      return;
    }
    
    IK_ASSERT(type_ == BodyType::DynamicBody);
    
    // Accumulate mass over all fixtures.
    Vec2 localCenter = Vec2_zero;
    for (Fixture* f = fixture_list_; f; f = f->next_) {
      if (f->density_ == 0.0f) {
        continue;
      }
      
      MassData massData;
      f->GetMassData(&massData);
      mass_ += massData.mass;
      localCenter += massData.mass * massData.center;
      inertia_ += massData.inertia;
    }
    
    // Compute center of mass.
    if (mass_ > 0.0f) {
      inv_mass_ = 1.0f / mass_;
      localCenter *= inv_mass_;
    }
    
    if (inertia_ > 0.0f && (flags_ & e_fixedRotationFlag) == 0) {
      // Center the inertia about the center of mass.
      inertia_ -= mass_ * Dot(localCenter, localCenter);
      IK_ASSERT(inertia_ > 0.0f);
      inv_inertia_ = 1.0f / inertia_;
      
    }
    else {
      inertia_ = 0.0f;
      inv_inertia_ = 0.0f;
    }
    
    // Move center of mass.
    Vec2 oldCenter = sweep_.c;
    sweep_.localCenter = localCenter;
    sweep_.c0 = sweep_.c = Mul(xf_, sweep_.localCenter);
    
    // Update center of mass velocity.
    linear_velocity_ += Cross(angular_velocity_, sweep_.c - oldCenter);
  }
  
  void Body::SetMassData(const MassData* massData) {
    IK_ASSERT(world_->IsLocked() == false);
    if (world_->IsLocked() == true) {
      return;
    }
    
    if (type_ != BodyType::DynamicBody) {
      return;
    }
    
    inv_mass_ = 0.0f;
    inertia_ = 0.0f;
    inv_inertia_ = 0.0f;
    
    mass_ = massData->mass;
    if (mass_ <= 0.0f) {
      mass_ = 1.0f;
    }
    
    inv_mass_ = 1.0f / mass_;
    
    if (massData->inertia > 0.0f && (flags_ & Body::e_fixedRotationFlag) == 0) {
      inertia_ = massData->inertia - mass_ * Dot(massData->center, massData->center);
      IK_ASSERT(inertia_ > 0.0f);
      inv_inertia_ = 1.0f / inertia_;
    }
    
    // Move center of mass.
    Vec2 oldCenter = sweep_.c;
    sweep_.localCenter =  massData->center;
    sweep_.c0 = sweep_.c = Mul(xf_, sweep_.localCenter);
    
    // Update center of mass velocity.
    linear_velocity_ += Cross(angular_velocity_, sweep_.c - oldCenter);
  }
  
  bool Body::ShouldCollide(const Body* other) const {
    // At least one body should be dynamic.
    if (type_ != BodyType::DynamicBody && other->type_ != BodyType::DynamicBody) {
      return false;
    }
    
    // Does a joint prevent collision?
    for (JointEdge* jn = joint_list_; jn; jn = jn->next) {
      if (jn->other == other) {
        if (jn->joint->collide_connected_ == false) {
          return false;
        }
      }
    }
    
    return true;
  }
  
  void Body::SetTransform(const Vec2& position, float angle) {
    IK_ASSERT(world_->IsLocked() == false);
    if (world_->IsLocked() == true) {
      return;
    }
    
    xf_.q.Set(angle);
    xf_.p = position;
    
    sweep_.c = Mul(xf_, sweep_.localCenter);
    sweep_.a = angle;
    
    sweep_.c0 = sweep_.c;
    sweep_.a0 = angle;
    
    BroadPhase* broadPhase = &world_->contact_manager.broad_phase_;
    for (Fixture* f = fixture_list_; f; f = f->next_) {
      f->Synchronize(broadPhase, xf_, xf_);
    }
    
    // Check for new contacts the next step
    world_->m_newContacts = true;
  }
  
  void Body::SynchronizeFixtures() {
    BroadPhase* broadPhase = &world_->contact_manager.broad_phase_;
    
    if (flags_ & Body::e_awakeFlag) {
      Transform xf1;
      xf1.q.Set(sweep_.a0);
      xf1.p = sweep_.c0 - Mul(xf1.q, sweep_.localCenter);
      
      for (Fixture* f = fixture_list_; f; f = f->next_) {
        f->Synchronize(broadPhase, xf1, xf_);
      }
    }
    else {
      for (Fixture* f = fixture_list_; f; f = f->next_) {
        f->Synchronize(broadPhase, xf_, xf_);
      }
    }
  }
  
  void Body::SetEnabled(bool flag) {
    IK_ASSERT(world_->IsLocked() == false);
    
    if (flag == IsEnabled()) {
      return;
    }
    
    if (flag) {
      flags_ |= e_enabledFlag;
      
      // Create all proxies.
      BroadPhase* broadPhase = &world_->contact_manager.broad_phase_;
      for (Fixture* f = fixture_list_; f; f = f->next_) {
        f->CreateProxies(broadPhase, xf_);
      }
      
      // Contacts are created at the beginning of the next
      world_->m_newContacts = true;
    }
    else {
      flags_ &= ~e_enabledFlag;
      
      // Destroy all proxies.
      BroadPhase* broadPhase = &world_->contact_manager.broad_phase_;
      for (Fixture* f = fixture_list_; f; f = f->next_) {
        f->DestroyProxies(broadPhase);
      }
      
      // Destroy the attached contacts.
      ContactEdge* ce = contact_list_;
      while (ce) {
        ContactEdge* ce0 = ce;
        ce = ce->next;
        world_->contact_manager.Destroy(ce0->contact);
      }
      contact_list_ = nullptr;
    }
  }
  
  void Body::SetFixedRotation(bool flag) {
    bool status = (flags_ & e_fixedRotationFlag) == e_fixedRotationFlag;
    if (status == flag) {
      return;
    }
    
    if (flag) {
      flags_ |= e_fixedRotationFlag;
    }
    else {
      flags_ &= ~e_fixedRotationFlag;
    }
    
    angular_velocity_ = 0.0f;
    
    ResetMassData();
  }

  BodyType Body::GetType() const {
    return type_;
  }
  
  const Transform& Body::GetTransform() const {
    return xf_;
  }
  
  const Vec2& Body::GetPosition() const {
    return xf_.p;
  }
  
  float Body::GetAngle() const {
    return sweep_.a;
  }
  
  const Vec2& Body::GetWorldCenter() const {
    return sweep_.c;
  }
  
  const Vec2& Body::GetLocalCenter() const {
    return sweep_.localCenter;
  }
  
  void Body::SetLinearVelocity(const Vec2& v) {
    if (type_ == BodyType::StaticBody)
    {
      return;
    }
    
    if (Dot(v,v) > 0.0f)
    {
      SetAwake(true);
    }
    
    linear_velocity_ = v;
  }
  
  const Vec2& Body::GetLinearVelocity() const {
    return linear_velocity_;
  }
  
  void Body::SetAngularVelocity(float w) {
    if (type_ == BodyType::StaticBody)
    {
      return;
    }
    
    if (w * w > 0.0f)
    {
      SetAwake(true);
    }
    
    angular_velocity_ = w;
  }
  
  float Body::GetAngularVelocity() const {
    return angular_velocity_;
  }
  
  float Body::GetMass() const {
    return mass_;
  }
  
  float Body::GetInertia() const {
    return inertia_ + mass_ * Dot(sweep_.localCenter, sweep_.localCenter);
  }
  
  void Body::GetMassData(MassData* data) const {
    data->mass = mass_;
    data->inertia = inertia_ + mass_ * Dot(sweep_.localCenter, sweep_.localCenter);
    data->center = sweep_.localCenter;
  }
  
  Vec2 Body::GetWorldPoint(const Vec2& localPoint) const {
    return Mul(xf_, localPoint);
  }
  
  Vec2 Body::GetWorldVector(const Vec2& localVector) const {
    return Mul(xf_.q, localVector);
  }
  
  Vec2 Body::GetLocalPoint(const Vec2& worldPoint) const {
    return MulT(xf_, worldPoint);
  }
  
  Vec2 Body::GetLocalVector(const Vec2& worldVector) const {
    return MulT(xf_.q, worldVector);
  }
  
  Vec2 Body::GetLinearVelocityFromWorldPoint(const Vec2& worldPoint) const {
    return linear_velocity_ + Cross(angular_velocity_, worldPoint - sweep_.c);
  }
  
  Vec2 Body::GetLinearVelocityFromLocalPoint(const Vec2& localPoint) const {
    return GetLinearVelocityFromWorldPoint(GetWorldPoint(localPoint));
  }
  
  float Body::GetLinearDamping() const {
    return linear_damping_;
  }
  
  void Body::SetLinearDamping(float linearDamping) {
    linear_damping_ = linearDamping;
  }
  
  float Body::GetAngularDamping() const {
    return angular_damping_;
  }
  
  void Body::SetAngularDamping(float angularDamping) {
    angular_damping_ = angularDamping;
  }
  
  float Body::GetGravityScale() const {
    return gravity_scale_;
  }
  
  void Body::SetGravityScale(float scale) {
    gravity_scale_ = scale;
  }
  
  void Body::SetBullet(bool flag) {
    if (flag)
    {
      flags_ |= e_bulletFlag;
    }
    else
    {
      flags_ &= ~e_bulletFlag;
    }
  }
  
  bool Body::IsBullet() const {
    return (flags_ & e_bulletFlag) == e_bulletFlag;
  }
  
  void Body::SetAwake(bool flag) {
    if (type_ == BodyType::StaticBody)
    {
      return;
    }
    
    if (flag)
    {
      flags_ |= e_awakeFlag;
      sleep_time_ = 0.0f;
    }
    else
    {
      flags_ &= ~e_awakeFlag;
      sleep_time_ = 0.0f;
      linear_velocity_.SetZero();
      angular_velocity_ = 0.0f;
      force_.SetZero();
      torque_ = 0.0f;
    }
  }
  
  bool Body::IsAwake() const {
    return (flags_ & e_awakeFlag) == e_awakeFlag;
  }
  
  bool Body::IsEnabled() const {
    return (flags_ & e_enabledFlag) == e_enabledFlag;
  }
  
  bool Body::IsFixedRotation() const {
    return (flags_ & e_fixedRotationFlag) == e_fixedRotationFlag;
  }
  
  void Body::SetSleepingAllowed(bool flag) {
    if (flag)
    {
      flags_ |= e_autoSleepFlag;
    }
    else
    {
      flags_ &= ~e_autoSleepFlag;
      SetAwake(true);
    }
  }
  
  bool Body::IsSleepingAllowed() const {
    return (flags_ & e_autoSleepFlag) == e_autoSleepFlag;
  }
  
  Fixture* Body::GetFixtureList() {
    return fixture_list_;
  }
  
  const Fixture* Body::GetFixtureList() const {
    return fixture_list_;
  }
  
  JointEdge* Body::GetJointList() {
    return joint_list_;
  }
  
  const JointEdge* Body::GetJointList() const {
    return joint_list_;
  }
  
  ContactEdge* Body::GetContactList() {
    return contact_list_;
  }
  
  const ContactEdge* Body::GetContactList() const {
    return contact_list_;
  }
  
  Body* Body::GetNext() {
    return next_;
  }
  
  const Body* Body::GetNext() const {
    return next_;
  }
  
  BodyUserData& Body::GetUserData() {
    return user_data_;
  }
  
  void Body::ApplyForce(const Vec2& force, const Vec2& point, bool wake) {
    if (type_ != BodyType::DynamicBody)
    {
      return;
    }
    
    if (wake && (flags_ & e_awakeFlag) == 0)
    {
      SetAwake(true);
    }
    
    // Don't accumulate a force if the body is sleeping.
    if (flags_ & e_awakeFlag)
    {
      force_ += force;
      torque_ += Cross(point - sweep_.c, force);
    }
  }
  
  void Body::ApplyForceToCenter(const Vec2& force, bool wake) {
    if (type_ != BodyType::DynamicBody)
    {
      return;
    }
    
    if (wake && (flags_ & e_awakeFlag) == 0)
    {
      SetAwake(true);
    }
    
    // Don't accumulate a force if the body is sleeping
    if (flags_ & e_awakeFlag)
    {
      force_ += force;
    }
  }
  
  void Body::ApplyTorque(float torque, bool wake) {
    if (type_ != BodyType::DynamicBody)
    {
      return;
    }
    
    if (wake && (flags_ & e_awakeFlag) == 0)
    {
      SetAwake(true);
    }
    
    // Don't accumulate a force if the body is sleeping
    if (flags_ & e_awakeFlag)
    {
      torque_ += torque;
    }
  }
  
  void Body::ApplyLinearImpulse(const Vec2& impulse, const Vec2& point, bool wake) {
    if (type_ != BodyType::DynamicBody)
    {
      return;
    }
    
    if (wake && (flags_ & e_awakeFlag) == 0)
    {
      SetAwake(true);
    }
    
    // Don't accumulate velocity if the body is sleeping
    if (flags_ & e_awakeFlag)
    {
      linear_velocity_ += inv_mass_ * impulse;
      angular_velocity_ += inv_inertia_ * Cross(point - sweep_.c, impulse);
    }
  }
  
  void Body::ApplyLinearImpulseToCenter(const Vec2& impulse, bool wake) {
    if (type_ != BodyType::DynamicBody)
    {
      return;
    }
    
    if (wake && (flags_ & e_awakeFlag) == 0)
    {
      SetAwake(true);
    }
    
    // Don't accumulate velocity if the body is sleeping
    if (flags_ & e_awakeFlag)
    {
      linear_velocity_ += inv_mass_ * impulse;
    }
  }
  
  void Body::ApplyAngularImpulse(float impulse, bool wake) {
    if (type_ != BodyType::DynamicBody)
    {
      return;
    }
    
    if (wake && (flags_ & e_awakeFlag) == 0)
    {
      SetAwake(true);
    }
    
    // Don't accumulate velocity if the body is sleeping
    if (flags_ & e_awakeFlag)
    {
      angular_velocity_ += inv_inertia_ * impulse;
    }
  }
  
  void Body::SynchronizeTransform() {
    xf_.q.Set(sweep_.a);
    xf_.p = sweep_.c - Mul(xf_.q, sweep_.localCenter);
  }
  
  void Body::Advance(float alpha) {
    // Advance to the new safe time. This doesn't sync the broad-phase.
    sweep_.Advance(alpha);
    sweep_.c = sweep_.c0;
    sweep_.a = sweep_.a0;
    xf_.q.Set(sweep_.a);
    xf_.p = sweep_.c - Mul(xf_.q, sweep_.localCenter);
  }
  
  World* Body::GetWorld() {
    return world_;
  }
  
  const World* Body::GetWorld() const {
    return world_;
  }
  

}
