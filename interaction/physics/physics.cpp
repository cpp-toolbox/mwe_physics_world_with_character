#include "physics.hpp"

// The Jolt headers don't include Jolt.h. Always include Jolt.h before including any other Jolt header.
// You can use Jolt.h in your precompiled header to speed up compilation.
#include "Jolt/Jolt.h"
#include "Jolt/Physics/Collision/Shape/CylinderShape.h"
#include "Jolt/Physics/Collision/Shape/MeshShape.h"

// Disable common warnings triggered by Jolt, you can use JPH_SUPPRESS_WARNING_PUSH / JPH_SUPPRESS_WARNING_POP to store and restore the warning state
JPH_SUPPRESS_WARNINGS

Physics::Physics() {
	this->initialize_engine();
	this->initialize_world_objects();
}

Physics::~Physics() {
	this->clean_up_world();
}

void Physics::initialize_engine() {
	JPH::RegisterDefaultAllocator();

	JPH::Trace = TraceImpl;
    JPH_IF_ENABLE_ASSERTS(JPH::AssertFailed = AssertFailedImpl;)

    JPH::Factory::sInstance = new JPH::Factory();
	JPH::RegisterTypes();

	// dynamic allocation, we don't worry about the rule of three since we never copy the physics system, there is only ever one instance
    temp_allocator = new JPH::TempAllocatorImpl(10 * 1024 * 1024);
    job_system = new JPH::JobSystemThreadPool(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, JPH::thread::hardware_concurrency() - 1);

    physics_system.Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, broad_phase_layer_interface, object_vs_broadphase_layer_filter, object_vs_object_layer_filter);

	body_activation_listener = new MyBodyActivationListener();
	contact_listener = new MyContactListener();

    physics_system.SetBodyActivationListener(body_activation_listener);
    physics_system.SetContactListener(contact_listener);

}

void Physics::initialize_world_objects() {
	JPH::BodyInterface &body_interface = physics_system.GetBodyInterface();

	// commented out the floor

	//JPH::BoxShapeSettings floor_shape_settings(JPH::Vec3(100.0f, 1.0f, 100.0f));

	//JPH::ShapeSettings::ShapeResult floor_shape_result = floor_shape_settings.Create();
	//JPH::ShapeRefC floor_shape = floor_shape_result.Get(); // We don't expect an error here, but you can check floor_shape_result for HasError() / GetError()
	//JPH::BodyCreationSettings floor_settings(floor_shape, JPH::RVec3(0.0_r, -1.0_r, 0.0_r), JPH::Quat::sIdentity(), JPH::EMotionType::Static, Layers::NON_MOVING);
	//JPH::Body *floor = body_interface.CreateBody(floor_settings); // Note that if we run out of bodies this can return nullptr
    //body_interface.AddBody(floor->GetID(), JPH::EActivation::DontActivate);
	//created_body_ids.push_back(floor->GetID());


	// commented out the sphere

	// JPH::BodyCreationSettings sphere_settings(new JPH::SphereShape(0.5f), JPH::RVec3(0.0_r, 2.0_r, 0.0_r), JPH::Quat::sIdentity(), JPH::EMotionType::Dynamic, Layers::MOVING);
    // sphere_id = body_interface.CreateAndAddBody(sphere_settings, JPH::EActivation::Activate);


	// created_body_ids.push_back(sphere_id);

    // body_interface.SetLinearVelocity(sphere_id, JPH::Vec3(0.0f, -5.0f, 0.0f));

	create_character();
}

/**
 * \brief For every mesh in this model, we create a physics object that represents the mesh
 */
void Physics::load_model_into_physics_world(Model* model) {

	JPH::BodyInterface &body_interface = physics_system.GetBodyInterface();

	for (int i = 0; i < model->meshes.size(); i++) {

		Mesh mesh = model->meshes[i];

		JPH::TriangleList triangles;

		// this only works if we know each face is a triangle,
		// TODO explain why it's guarenteed to be triangles
		for (int j = 0; j < mesh.indices.size(); j += 3) {
			unsigned int j1 = mesh.indices[j];
			unsigned int j2 = mesh.indices[j + 1];
			unsigned int j3 = mesh.indices[j + 2];

			// TODO use the helper to convert from glm to jph
			glm::vec3 temp_v1 = mesh.vertices[j1].position;
			JPH::Float3 v1 = JPH::Float3(temp_v1.x, temp_v1.y, temp_v1.z);

			glm::vec3 temp_v2 = mesh.vertices[j2].position;
			JPH::Float3 v2 = JPH::Float3(temp_v2.x, temp_v2.y, temp_v2.z);

			glm::vec3 temp_v3 = mesh.vertices[j3].position;
			JPH::Float3 v3 = JPH::Float3(temp_v3.x, temp_v3.y, temp_v3.z);

			JPH::Triangle tri = JPH::Triangle(v1, v2, v3);

			triangles.push_back(tri);
		}

		JPH::MeshShapeSettings settings = JPH::MeshShapeSettings(triangles);

		JPH::Ref<JPH::Shape> mesh_shape;

		// Create shape
		JPH::Shape::ShapeResult result = settings.Create();
		if (result.IsValid()) {
			mesh_shape = result.Get();
		} else {
			// TODO Error handling
		}

		JPH::BodyCreationSettings mesh_settings(mesh_shape, JPH::RVec3(0.0_r, 0.0_r, 0.0_r), JPH::Quat::sIdentity(), JPH::EMotionType::Static, Layers::NON_MOVING);
		JPH::Body *mesh_body = body_interface.CreateBody(mesh_settings); // Note that if we run out of bodies this can return nullptr
		body_interface.AddBody(mesh_body->GetID(), JPH::EActivation::DontActivate);
		created_body_ids.push_back(mesh_body->GetID());

	 }
}

/**
 * \brief create character controller for a user
 * \todo do I have to account for dynamic memory? Come back when you know what ref is
 */
void Physics::create_character() {
	JPH::Ref<JPH::CharacterVirtualSettings> settings = new JPH::CharacterVirtualSettings();
	settings->mShape = new JPH::CylinderShape(0.5f * this->character_height + this->character_radius, this->character_radius);

	character = new JPH::CharacterVirtual(settings, JPH::RVec3::sZero(), JPH::Quat::sIdentity(), &physics_system);
}

/**
 * \brief updates the objects part of this physics simulation
 */
void Physics::update(float delta_time) {

	JPH::Vec3 ch_up = character->GetUp();
	// std::cout << "Character UP = (" << ch_up.GetX() << ", " << ch_up.GetY() << ", " << ch_up.GetZ() << ")" << std::endl;

	// Settings for our update function
	JPH::CharacterVirtual::ExtendedUpdateSettings update_settings;
	//update_settings.mStickToFloorStepDown = character->GetUp() * update_settings.mStickToFloorStepDown.Length();
	//update_settings.mWalkStairsStepUp = character->GetUp() * update_settings.mWalkStairsStepUp.Length();

	// Update the character position
	character->ExtendedUpdate(delta_time, - character->GetUp() * physics_system.GetGravity().Length(), update_settings,
		physics_system.GetDefaultBroadPhaseLayerFilter(Layers::MOVING), physics_system.GetDefaultLayerFilter(Layers::MOVING),
		 { }, { }, *temp_allocator);


	physics_system.Update(delta_time, cCollisionSteps, temp_allocator, job_system);

}

void Physics::clean_up_world() {
	JPH::BodyInterface &body_interface = physics_system.GetBodyInterface();

	for (auto body_id : created_body_ids) {
		body_interface.RemoveBody(body_id);
		body_interface.DestroyBody(body_id);
	}

	JPH::UnregisterTypes();

	// de-allocate dynamic memory
	delete temp_allocator;
	delete job_system;
	delete body_activation_listener;
	delete contact_listener;

    delete JPH::Factory::sInstance;
	JPH::Factory::sInstance = nullptr;

	std::cout << "successfully cleaned up world" << std::endl;
}


// void Physics::print_positions() {
	//print positions of all objects
	//for (int j = dynamics_world->getNumCollisionObjects() - 1; j >= 0; j--)
	//{
	//	btCollisionObject* obj = dynamics_world->getCollisionObjectArray()[j];
	//	btRigidBody* body = btRigidBody::upcast(obj);
	//	btTransform trans;
	//	if (body && body->getMotionState()) {
	//		body->getMotionState()->getWorldTransform(trans);
	//	} else {
	//		trans = obj->getWorldTransform();
	//	}
	//	printf("world pos object %d = %f,%f,%f\n", j, float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));
	//}
//}