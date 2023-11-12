#include "CheckpointController.h"
#include <algorithm>
#include <ranges>

void CheckpointController::addCheckpoint(PhysicalObject::fVec3 position, PhysicalObject::fVec3 scale, DirectX::XMVECTOR direction, float distGround)
{
	Checkpoint* cp = new Checkpoint{};
	cp->indexCheckpoint = static_cast<int>(m_checkpoints.size());
	cp->distGround = distGround;
	cp->passed = cp->indexCheckpoint == 0;
	
	cp->direction = DirectX::XMVector3NormalizeEst(direction);
	cp->triggerBox = new TriggerBox{ position, scale };
	cp->triggerBox->setTriggerCallback(std::move([cp, this]() {
		if(!cp->passed)
		{
			cp->passed = true;
			indexLastCP = cp->indexCheckpoint;
			//std::cout << "Checkpoint " << cp->indexCheckpoint << " passed" << std::endl;
		}
		}));
	m_checkpoints.push_back(cp);

}

void CheckpointController::addCheckpointFromJson(const std::vector<FormatJson>& checkpointInfos)
{
	std::ranges::for_each(checkpointInfos, [&](const FormatJson& infos) {
			PhysicalObject::fVec3 position{ DirectX::XMVectorGetX(infos.positionObj),  DirectX::XMVectorGetY(infos.positionObj),  DirectX::XMVectorGetZ(infos.positionObj) };
			PhysicalObject::fVec3 scale = { DirectX::XMVectorGetX(infos.scaleObj),  DirectX::XMVectorGetY(infos.scaleObj),  DirectX::XMVectorGetZ(infos.scaleObj) };

			addCheckpoint(position, scale, infos.forwardObj, DirectX::XMVectorGetW(infos.positionObj));
		});
}

void CheckpointController::validAllCheckpoints()
{
	for (Checkpoint* cp : m_checkpoints)
		cp->passed = true;
	
}

void CheckpointController::resetAllCheckpoints()
{
	for (Checkpoint* cp : m_checkpoints) cp->passed = false;
	m_checkpoints[0]->passed = true;
	indexLastCP = 0;
}


void CheckpointController::deleteCheckpoints()
{
	m_checkpoints.clear();
}
bool CheckpointController::allCheckpointsPassed() const
{
	return std::all_of(m_checkpoints.begin(), m_checkpoints.end(), [](const Checkpoint* cp) { return cp->passed; });
}

DirectX::XMVECTOR CheckpointController::getPositionLastCP() const
{
	DirectX::XMVECTOR pos = m_checkpoints[indexLastCP]->triggerBox->getTransform().getPosition(); 
	return DirectX::XMVectorSet(DirectX::XMVectorGetX(pos), DirectX::XMVectorGetY(pos) - m_checkpoints[indexLastCP]->distGround, DirectX::XMVectorGetZ(pos), 0);
}
DirectX::XMVECTOR CheckpointController::getDirectionLastCP() const
{	
	return m_checkpoints[indexLastCP]->direction;
}
