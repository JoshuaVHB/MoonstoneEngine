#pragma once
#include <vector>
#include "../../Physics/World/TriggerBox.h"
#include "../../Physics/World/PhysicalObject.h"
#include "../../Platform/IO/JsonParser.h"
struct Checkpoint {
	TriggerBox* triggerBox;
	bool passed;
	int indexCheckpoint;
	float distGround;
	~Checkpoint() {
		delete triggerBox;
	}
};

class CheckpointController
{
	std::vector<Checkpoint*> m_checkpoints;
	int indexLastCP{};

public:
	CheckpointController() = default;	
	~CheckpointController() {
		deleteCheckpoints();
	}
	
	void addCheckpoint(PhysicalObject::fVec3 position, PhysicalObject::fVec3 scale, float distGround = 0.f );

	void addCheckpointFromJson(const std::vector<FormatJson> & checkpointInfos);

	void validAllCheckpoints();

	void resetAllCheckpoints();

	void deleteCheckpoints();

	[[nodiscard]] bool allCheckpointsPassed() const;

	[[nodiscard]] DirectX::XMVECTOR  getPositionLastCP() const;
};

