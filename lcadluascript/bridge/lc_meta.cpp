#include <cad/interface/metatype.h>
#include <cad/meta/block.h>
#include <cad/meta/metalinewidth.h>
#include <cad/base/metainfo.h>
#include <cad/meta/layer.h>
#include <cad/meta/customentitystorage.h>
#include "lc_meta.h"

void import_lc_meta_namespace(sol::state & luaVM) {
    sol::table lc = luaVM["lc"];
    lc["meta"] = luaVM.create_table();
    sol::table meta = lc["meta"];

    meta.new_usertype<lc::meta::MetaType>(
            "MetaType",
            "id", &lc::meta::MetaType::id
            );

    meta.new_usertype<lc::meta::DocumentMetaType>(
            "DocumentMetaType",
            sol::base_classes, sol::bases<lc::meta::MetaType>(),
            "name", &lc::meta::DocumentMetaType::name
            );

    meta.new_usertype<lc::meta::EntityMetaType>(
            "EntityMetaType",
            sol::base_classes, sol::bases<lc::meta::MetaType>(),
            "metaTypeID", &lc::meta::EntityMetaType::metaTypeID
            );

    meta.new_usertype<lc::meta::DxfLinePattern>(
            "DxfLinePattern",
            sol::base_classes, sol::bases<lc::meta::DocumentMetaType, lc::meta::EntityMetaType>(),
            "LCMETANAME", &lc::meta::DxfLinePattern::LCMETANAME,
            "description", &lc::meta::DxfLinePattern::description,
            "id", &lc::meta::DxfLinePattern::id,
            "metaTypeID", &lc::meta::DxfLinePattern::metaTypeID
            );

    meta.new_usertype<lc::meta::DxfLinePatternByValue>(
            "DxfLinePatternByValue",
            sol::constructors<lc::meta::DxfLinePatternByValue(std::string, std::string, std::vector<double>, double)>(),
            sol::base_classes, sol::bases<lc::meta::DxfLinePattern>(),
            "calculatePathLength", &lc::meta::DxfLinePatternByValue::calculatePathLength,
            "description", &lc::meta::DxfLinePatternByValue::description,
            "generatePattern", &lc::meta::DxfLinePatternByValue::generatePattern,
            "lcPattern", &lc::meta::DxfLinePatternByValue::lcPattern,
            "length", &lc::meta::DxfLinePatternByValue::length,
            "name", &lc::meta::DxfLinePatternByValue::name,
            "path", &lc::meta::DxfLinePatternByValue::path
            );

    meta.new_usertype<lc::meta::DxfLinePatternByBlock>(
            "DxfLinePatternByBlock",
            sol::base_classes, sol::bases<lc::meta::DxfLinePattern>(),
            "name", &lc::meta::DxfLinePatternByBlock::name
            );

    meta.new_usertype<lc::meta::MetaInfo>(
            "MetaInfo",
            "new", []() { return std::make_shared<lc::meta::MetaInfo>(); },
            "add", &lc::meta::MetaInfo::add,
            "create", &lc::meta::MetaInfo::create
            );

    meta.new_usertype<lc::meta::MetaColor>(
            "MetaColor",
            sol::base_classes, sol::bases<lc::meta::EntityMetaType>(),
            "LCMETANAME", &lc::meta::MetaColor::LCMETANAME,
            "metaTypeID", &lc::meta::MetaColor::metaTypeID
            );

    meta.new_usertype<lc::meta::MetaColorByValue>(
            "MetaColorByValue",
            sol::constructors<
            lc::meta::MetaColorByValue(), lc::meta::MetaColorByValue(const lc::meta::MetaColorByValue &), 
            lc::meta::MetaColorByValue(const lc::Color &),
            lc::meta::MetaColorByValue(double, double, double, double)
            >(),
            sol::base_classes, sol::bases<lc::meta::MetaColor>(),
            "alpha", &lc::meta::MetaColorByValue::alpha,
            "alphaI", &lc::meta::MetaColorByValue::alphaI,
            "blue", &lc::meta::MetaColorByValue::blue,
            "blueI", &lc::meta::MetaColorByValue::blueI,
            "color", &lc::meta::MetaColorByValue::color,
            "green", &lc::meta::MetaColorByValue::green,
            "greenI", &lc::meta::MetaColorByValue::greenI,
            "id", &lc::meta::MetaColorByValue::id,
            "red", &lc::meta::MetaColorByValue::red,
            "redI", &lc::meta::MetaColorByValue::redI
            );

    meta.new_usertype<lc::meta::MetaColorByBlock>(
            "MetaColorByBlock",
            sol::constructors<lc::meta::MetaColorByBlock()>(),
            sol::base_classes, sol::bases<lc::meta::MetaColor>(),
            "id", &lc::meta::MetaColorByBlock::id
            );

    meta.new_usertype<lc::meta::MetaLineWidth>(
            "MetaLineWidth", 
            sol::base_classes, sol::bases<lc::meta::EntityMetaType>(),
            "LCMETANAME", &lc::meta::MetaLineWidth::LCMETANAME,
            "id", &lc::meta::MetaLineWidth::id
            );

    meta.new_usertype<lc::meta::MetaLineWidthByBlock>(
            "MetaLineWidthByBlock",
            sol::constructors<lc::meta::MetaLineWidthByBlock()>(),
            sol::base_classes, sol::bases<lc::meta::MetaLineWidth>(),
            "metaTypeID", &lc::meta::MetaLineWidthByBlock::metaTypeID
            );

    meta.new_usertype<lc::meta::MetaLineWidthByValue>(
            "MetaLineWidthByValue",
            sol::constructors<lc::meta::MetaLineWidthByValue(), lc::meta::MetaLineWidthByValue(const double)>(),
            sol::base_classes, sol::bases<lc::meta::MetaLineWidth, lc::meta::DocumentMetaType>(),
            "id", &lc::meta::MetaLineWidthByValue::id,
            "metaTypeID", &lc::meta::MetaLineWidthByValue::metaTypeID,
            "name", &lc::meta::MetaLineWidthByValue::name,
            "width", &lc::meta::MetaLineWidthByValue::width
            );

    meta.new_usertype<lc::meta::Block>("Block",
            sol::constructors<lc::meta::Block(std::string, lc::geo::Coordinate)>(),
            sol::base_classes, sol::bases<lc::meta::DocumentMetaType>(),
            "base", &lc::meta::Block::base,
            "id", &lc::meta::Block::id,
            "name", &lc::meta::Block::name
            );

    meta.new_usertype<lc::meta::Layer>(
            "Layer",
            sol::constructors<lc::meta::Layer(std::string, const lc::meta::MetaLineWidthByValue &, const lc::Color &, lc::meta::DxfLinePatternByValue_CSPtr, bool)>(),
            sol::base_classes, sol::bases<lc::meta::EntityMetaType, lc::meta::DocumentMetaType>(),
            "LCMETANAME", &lc::meta::Layer::LCMETANAME,
            "color", &lc::meta::Layer::color,
            "id", &lc::meta::Layer::id,
            "isFrozen", &lc::meta::Layer::isFrozen,
            "linePattern", &lc::meta::Layer::linePattern,
            "lineWidth", &lc::meta::Layer::lineWidth,
            "metaTypeID", &lc::meta::Layer::metaTypeID,
            "name", &lc::meta::Layer::name
            );

    meta.new_usertype<lc::meta::CustomEntityStorage>(
            "CustomEntityStorage",
            sol::constructors<lc::meta::CustomEntityStorage(std::string, std::string, lc::geo::Coordinate, std::map<std::string, std::string>)>(),
            sol::base_classes, sol::bases<lc::meta::Block>(),
            "entityName", &lc::meta::CustomEntityStorage::entityName,
            "param", &lc::meta::CustomEntityStorage::param,
            "params", &lc::meta::CustomEntityStorage::params,
            "pluginName", &lc::meta::CustomEntityStorage::pluginName,
            "setParam", &lc::meta::CustomEntityStorage::setParam
            );
}
