/* Copyright (c) <2003-2016> <Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely
*/

#include "dAnimationStdAfx.h"
#include "dAnimationModelManager.h"


dAnimationModelManager::dAnimationModelManager(NewtonWorld* const world, const char* const name)
	:dCustomParallelListener(world, name)
	,m_controllerList()
	,m_timestep(0.0f)
{
}

dAnimationModelManager::~dAnimationModelManager()
{
}

void dAnimationModelManager::AddModel(dAnimationJointRoot* const model)
{
	dAssert(!model->m_managerNode);
	model->m_manager = this;
	model->m_managerNode = m_controllerList.Append(model);
}

void dAnimationModelManager::RemoveModel(dAnimationJointRoot* const model)
{
	dAssert(model->m_managerNode);
	dAssert(model->m_manager == this);
	dAssert(model->m_managerNode->GetInfo() == model);
	m_controllerList.Remove(model->m_managerNode);
	model->m_manager = NULL;
	model->m_managerNode = NULL;
}

dAnimationJoint* dAnimationModelManager::GetFirstJoint(dAnimationJoint* const root) const
{
	dAnimationJoint* joint = root;
	while (joint->GetChildren().GetCount()) {
		joint = joint->GetChildren().GetFirst()->GetInfo();
	}
	return joint;
}

dAnimationJoint* dAnimationModelManager::GetNextJoint(dAnimationJoint* const joint) const
{
	/*
	dAssert ()
	if (joint->m_node->GetNext()->GetNode) {
	}
	if (!joint->GetParent()) {
		return NULL;
	}
	dAnimationJoint* const sibling = ->;
*/
	return NULL;
}


void dAnimationModelManager::OnDestroy()
{
	while (m_controllerList.GetFirst()) {
		dAnimationJointRoot* const model = m_controllerList.GetFirst()->GetInfo();
		dAssert(model->m_managerNode == m_controllerList.GetFirst());
		delete model;
	}
}

void dAnimationModelManager::OnPreUpdate(dAnimationJointRoot* const model, dFloat timestep)
{
	D_TRACKTIME();
	model->PreUpdate(timestep);
}

void dAnimationModelManager::PreUpdate(dFloat timestep, int threadID)
{
	D_TRACKTIME();
	NewtonWorld* const world = GetWorld();
	const int threadCount = NewtonGetThreadsCount(world);

	dList<dAnimationJointRoot*>::dListNode* node = m_controllerList.GetFirst();
	for (int i = 0; i < threadID; i++) {
		node = node ? node->GetNext() : NULL;
	}
	if (node) {
		dAnimationJointRoot* const model = node->GetInfo();
		//model->PreUpdate(timestep);
		OnPreUpdate(model, timestep);
		do {
			for (int i = 0; i < threadCount; i++) {
				node = node ? node->GetNext() : NULL;
			}
		} while (node);
	}
}

void dAnimationModelManager::PostUpdate(dFloat timestep, int threadID)
{
	D_TRACKTIME();
	NewtonWorld* const world = GetWorld();
	const int threadCount = NewtonGetThreadsCount(world);

	dList<dAnimationJointRoot*>::dListNode* node = m_controllerList.GetFirst();
	for (int i = 0; i < threadID; i++) {
		node = node ? node->GetNext() : NULL;
	}
	if (node) {
		dAnimationJointRoot* const model = node->GetInfo();
		OnPostUpdate(model, timestep);
		model->UpdateTransforms(timestep);
		do {
			for (int i = 0; i < threadCount; i++) {
				node = node ? node->GetNext() : NULL;
			}
		} while (node);
	}
}
