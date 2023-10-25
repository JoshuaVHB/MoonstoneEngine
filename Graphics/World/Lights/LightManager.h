#pragma once


#include <vector>
#include <memory>
#include <variant>

#include "Lights.h"
#include "..\..\abstraction\Effect.h"

/* Add this to the shader code to attach lights correctly :

struct DirectionalLight {
	float4 direction;
	float4 ambiant;
	float4 diffuse;
	float	strength;
};

struct PointLight {

	float4 range;
	float4 position;
	float4 ambiant;
	float4 diffuse;
	float falloff;
};


struct SpotLight {

	float4 direction;
	float4 ambiant;
	float4 diffuse;
	float4 position;

	float falloff;
	float radius;
	float strength;
};

cbuffer LightManager {

	Spotlight			spotlights[64];
	PointLight			pointslights[64];
	DirectionalLight	dirlights[64];
};*/

class LightManager 
{
private:

	std::vector<DirectionalLight> m_dirs;
	std::vector<PointLight> m_point;
	std::vector<SpotLight> m_spots;

public:

	LightManager() {}

	void addSpotlight()		{}
	void addDirlight()		{}
	void addPointlight(PointLight&& pl)
	{
		m_point.push_back(pl);
	}

	std::vector<hlsl_GenericLight> convertLights() const noexcept
	{
		std::vector<hlsl_GenericLight> res;
		for (const SpotLight& sl : m_spots)
		{

			res.push_back(hlsl_GenericLight{

				sl.direction,
				{},
				sl.position,
				sl.ambiant,
				sl.diffuse,
				sl.falloff,
				sl.radius,
				sl.strength,
				static_cast<float>(sl.isOn),
				static_cast<uint32_t>(sl.type)
			});
		}
		for (const PointLight& pl : m_point)
		{

			res.push_back(hlsl_GenericLight{

				{},
				{},
				pl.position,
				pl.ambiant,
				pl.diffuse,
				pl.falloff,
				pl.falloff,
				{},
				static_cast<float>(pl.isOn),
				static_cast<uint32_t>(pl.type)

			});
		}

		for (const DirectionalLight& dl : m_dirs)
		{

			res.push_back(hlsl_GenericLight{

				dl.direction,
				{},
				{},
				dl.ambiant,
				dl.diffuse,
				{},
				{},
				dl.strength,
				static_cast<float>(dl.isOn),
				static_cast<uint32_t>(dl.type)
			});
		}

		return res;
		
	}

	std::vector<std::string> getAllLightsDebugName() const 
	{

		std::vector<std::string> res;

		for (int i = 0; i < m_point.size(); ++i)
		{
			res.push_back("Pointlight " + std::to_string(i));
		}

		for (int i = 0; i < m_dirs.size(); ++i)
		{
			res.push_back("Directionallight " + std::to_string(i));
		}

		for (int i = 0; i < m_spots.size(); ++i)
		{
			res.push_back("Spotlight " + std::to_string(i));
		}
		return res;
	}


public:

	void showDebugWindow()
	{
		ImGui::Begin("Lights");
		if(ImGui::Button("Add new point light"))
		{

			m_point.push_back(PointLight{ {1,1,1},{}, {1,1,1}, {1,1,1}, 1, true });
		}


		static auto names = getAllLightsDebugName();
		static const char* current_item = nullptr;
		int currentItemIndex = 0;


		if (ImGui::BeginCombo("All lights", current_item, ImGuiComboFlags_NoArrowButton ))
		{
			for (int n = 0; n < names.size(); n++)
			{
				bool is_selected = (current_item == names[n].c_str()); // You can store your selection however you want, outside or inside your objects
				if (ImGui::Selectable(names[n].c_str(), is_selected))
				{					
					current_item = names[n].c_str();
					currentItemIndex = n;
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
			}
			ImGui::EndCombo();
		}


		if (current_item)
		{			
			ImGui::BeginChild("Current Selected");

			switch (current_item[0])
			{
				case 'P':
					{						
						int tmpIndex = currentItemIndex - 0;
						ImGui::DragFloat3(("Position ##" +	std::to_string(tmpIndex)).c_str(), &m_point[tmpIndex].position.vector4_f32[0]);
						ImGui::DragFloat3(("Range ##" + std::to_string(tmpIndex)).c_str(), &m_point[tmpIndex].range.vector4_f32[0]);
						ImGui::DragFloat(("falloff ##" + std::to_string(tmpIndex)).c_str(), &m_point[tmpIndex].falloff);
						ImGui::ColorEdit3(("Ambiant ##" + std::to_string(tmpIndex)).c_str(), &m_point[tmpIndex].ambiant.vector4_f32[0]);
						ImGui::ColorEdit3(("Diffuse ##" + std::to_string(tmpIndex)).c_str(), &m_point[tmpIndex].diffuse.vector4_f32[0]);
						ImGui::Checkbox(("IsOn ##" + std::to_string(tmpIndex)).c_str(), &m_point[tmpIndex].isOn);
					break;
					}
				case 'S':
					{
						
					int tmpIndex = currentItemIndex - m_point.size();
					break;

					}
				case 'D':
					break;
			default:
				break;
			}

			ImGui::EndChild();
		}

		ImGui::End();
	}

};