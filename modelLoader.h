/*======================================================================
VulkanPBR_AcornForest : modelLoader.h
Author:			Sim Luigi
Last Modifier:  2020.12.16
=======================================================================*/

#pragma once
#include <string>
#include <vector>
#include "tiny_gltf.h"

static std::string GetFilePathExtension(const std::string &FileName);
static std::string PrintMode(int mode);
static std::string PrintTarget(int target);
static std::string PrintType(int ty);
static std::string PrintComponentType(int ty);
static std::string PrintWrapMode(int mode);
static std::string PrintFilterMode(int mode);
static std::string PrintIntArray(const std::vector<int> &arr);
static std::string PrintFloatArray(const std::vector<double> &arr);
static std::string Indent(const int indent);
static std::string PrintParameterValue(const tinygltf::Parameter &param);
static std::string PrintValue(const std::string &name,
	const tinygltf::Value &value, const int indent,
	const bool tag);

static void DumpNode(const tinygltf::Node &node, int indent);
static void DumpStringIntMap(const std::map<std::string, int> &m, int indent);
static void DumpExtensions(const tinygltf::ExtensionMap &extension,
	const int indent);
static void DumpPrimitive(const tinygltf::Primitive &primitive, int indent);
static void DumpTextureInfo(const tinygltf::TextureInfo &texinfo,
	const int indent);
static void DumpNormalTextureInfo(const tinygltf::NormalTextureInfo &texinfo,
	const int indent);
static void DumpOcclusionTextureInfo(
	const tinygltf::OcclusionTextureInfo &texinfo, const int indent);
static void DumpPbrMetallicRoughness(const tinygltf::PbrMetallicRoughness &pbr,
	const int indent);
static void Dump(const tinygltf::Model &model);

int loadGltf(int argc, char **argv);