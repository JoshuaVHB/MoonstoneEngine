#include "CheckpointController.h"
#include <algorithm>
#include <ranges>
void CheckpointController::addCheckpoint(PhysicalObject::fVec3 position, PhysicalObject::fVec3 scale)
{
	Checkpoint* cp = new Checkpoint{};
	cp->indexCheckpoint = static_cast<int>(m_checkpoints.size());
	cp->distGround = distGround;
	cp->passed = cp->indexCheckpoint == 0;
	cp->triggerBox = new TriggerBox{ position, scale };
	cp->triggerBox->setTriggerCallback(std::move([cp, this]() {
		if(!cp->passed)
		{
			cp->passed = true;
			indexLastCP = cp->indexCheckpoint;
			std::cout << "Checkpoint " << cp->indexCheckpoint << " passed" << std::endl;
		}
		}));
	m_checkpoints.push_back(cp);

}

void CheckpointController::addCheckpointFromJson(const std::vector<FormatJson>& checkpointInfos)
{
	std::ranges::for_each(checkpointInfos, [&](const FormatJson& infos) {
			PhysicalObject::fVec3 position{ DirectX::XMVectorGetX(infos.positionObj),  DirectX::XMVectorGetY(infos.positionObj),  DirectX::XMVectorGetZ(infos.positionObj) };
			PhysicalObject::fVec3 scale = { DirectX::XMVectorGetX(infos.scaleObj),  DirectX::XMVectorGetY(infos.scaleObj),  DirectX::XMVectorGetZ(infos.scaleObj) };

			addCheckpoint(position, scale);
		});
}

void CheckpointController::validAllCheckpoints()
{
	for (Checkpoint* cp : m_checkpoints)
		cp->passed = true;
	
}

void CheckpointController::resetAllCheckpoints()
{
	for (Checkpoint* cp : m_checkpoints)
		cp->passed = false;
}

void CheckpointController::deleteCheckpoints()
{
	m_checkpoints.clear();
}
bool CheckpointController::allCheckpointsPassed() const
{
	return std::all_of(m_checkpoints.begin(), m_checkpoints.end(), [](const Checkpoint* cp) { return cp->passed; });
}
