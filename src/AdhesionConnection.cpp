#include "AdhesionConnection.hpp"
#include <iostream>

Cell* SpringConnection::get_other_cell(Cell* current_cell){
	if (is_disconnected) return nullptr;
	if (c1 == current_cell) return c2;
	else return c1;
	
}
	
Position2D SpringConnection::get_conn_relative_location(Cell* target_cell){
	if (is_disconnected) return Position2D{0.0, 0.0};
	if (c1 == target_cell){
		return Position2D{c1->size * static_cast<float>(cos(c1->angle_rads + c1_angle_rads)), c1->size * -static_cast<float>(sin(c1->angle_rads + c1_angle_rads))};
	}
	else return Position2D{c2->size * static_cast<float>(cos(c2->angle_rads + c2_angle_rads)), c2->size * -static_cast<float>(sin(c2->angle_rads + c2_angle_rads))};
	
	
	
}
Position2D SpringConnection::get_conn_global_location(Cell* target_cell){
	if (is_disconnected) return Position2D{0.0, 0.0};
	Position2D rel = get_conn_relative_location(target_cell);
	
	if (c1 == target_cell){
		return rel + c1->pos;
	}
	else return rel + c2->pos;
	
}

void SpringConnection::apply_force_to_cell(Cell* target_cell){
	if (is_disconnected) return;
	if (c1 == target_cell){
		c1->add_force(next_c1_force);
		c1->add_torque(next_c1_torque);
	}
	else {
		c2->add_force(next_c2_force);
		c2->add_torque(next_c2_torque);
	}
	
	
}

void SpringConnection::disconnect_connection(){
	
	is_disconnected = true;
	
	//make sure you can no longer accsess connected cells
	c1 = nullptr;
	c2 = nullptr;
	
}
bool SpringConnection::get_is_disconnected(){
	
	return is_disconnected;
	
}


void SpringConnection::simulation_step(){
	if (is_disconnected) return;
	
	
	Position2D dif = get_conn_global_location(c2) - get_conn_global_location(c1);
	
	float length = std::sqrt(dif.x * dif.x + dif.y * dif.y);
	if (length == 0){
		next_c1_force = Position2D{0, 0};
		next_c2_force = Position2D{0, 0};
		
		next_c1_torque = 0;
		next_c2_torque = 0;
		return;
	}
	
	Position2D normal = Position2D{dif.x / length, dif.y/length};
	const float restLength = 0.5*.14;
	float displacement = length - restLength;
	const float k = .5*.14;
	Position2D springForce = Position2D{-k * displacement * normal.x, -k * displacement * normal.y};
	
	
	
	Position2D c1_connection_point = get_conn_relative_location(c1);
	Position2D c2_connection_point = get_conn_relative_location(c2);
	
	Position2D c1_vel_sum = c1->vel + Position2D{c1->angle_vel * c1_connection_point.y, -c1->angle_vel * c1_connection_point.x};
	Position2D c2_vel_sum = c2->vel + Position2D{c2->angle_vel * c2_connection_point.y, -c2->angle_vel * c2_connection_point.x};
	
	Position2D rel_vel = c2_vel_sum - c1_vel_sum;
	const float damping = .5*.14;
	float dampingForceMag = -(rel_vel.x * normal.x + rel_vel.y * normal.y) * damping;
	Position2D dampingForce = Position2D{dampingForceMag * normal.x, dampingForceMag * normal.y};
	Position2D sum_force = springForce + dampingForce;
	
	
	
	
	next_c1_force = Position2D{-1.0, -1.0} * sum_force;
	next_c2_force = sum_force; 
	
	
	next_c1_torque = (c1_connection_point.x * sum_force.y - c1_connection_point.y * sum_force.x);// + relativeOmega*angularDamping;
	next_c2_torque = -(c2_connection_point.x * sum_force.y - c2_connection_point.y * sum_force.x);// - relativeOmega*angularDamping;
	
	
	
	
	
	
}


SpringConnection::SpringConnection(Cell* c1, Cell* c2){
	
	this->c1 = c1;
	this->c2 = c2;
	
	
	
	Position2D vector_dif = c1->pos - c2->pos;
				
	if (vector_dif.x == 0.0f) vector_dif.x = .0001;
	float port_angle = std::atan(-vector_dif.y/vector_dif.x);
	
	if (vector_dif.x >= 0){
		this->c1_angle_rads = M_PI+port_angle - c1->angle_rads;
		this->c2_angle_rads = port_angle - c2->angle_rads;
	}
	else {
		this->c1_angle_rads = port_angle - c1->angle_rads;
		this->c2_angle_rads = M_PI+port_angle - c2->angle_rads;
	}
	
	
	
}

SpringConnection::~SpringConnection(){
	 
}


AdhesionConnection::AdhesionConnection(Cell* c1, Cell* c2) : SpringConnection(c1, c2){
	
	
}