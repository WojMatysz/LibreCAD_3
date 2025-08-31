#include <cad/operations/undoable.h>
#include <cad/operations/builder.h>
#include <cad/operations/documentoperation.h>
#include <cad/operations/entityops.h>
#include <cad/operations/layerops.h>
#include <cad/operations/linepatternops.h>
#include <cad/operations/blockops.h>
#include <cad/operations/entitybuilder.h>
#include "lc_operation.h"

void import_lc_operation_namespace(sol::state & luaVM) {
    sol::table lc = luaVM["lc"];
    lc["operation"] = luaVM.create_table();
    sol::table operation = lc["operation"];

    operation.new_usertype<lc::operation::Undoable>(
            "Undoable",
            "redo", &lc::operation::Undoable::redo,
            "text", &lc::operation::Undoable::text,
            "undo", &lc::operation::Undoable::undo
            );

    operation.new_usertype<lc::operation::DocumentOperation>(
            "DocumentOperation",
            sol::base_classes, sol::bases<lc::operation::Undoable>(),
            "document", &lc::operation::DocumentOperation::document,
            "execute", &lc::operation::DocumentOperation::execute
            );

    operation.new_usertype<lc::operation::Builder>(
            "Builder",
            sol::constructors<lc::operation::Builder(lc::storage::Document_SPtr, const std::string &)>(),
            sol::base_classes, sol::bases<lc::operation::DocumentOperation>(),
            "append", &lc::operation::Builder::append,
            "redo", &lc::operation::Builder::redo,
            "undo", &lc::operation::Builder::undo
            );

    operation.new_usertype<lc::operation::Base>(
            "Base",
            "process", &lc::operation::Base::process
            );

    operation.new_usertype<lc::operation::Loop>(
            "Loop",
            sol::base_classes, sol::bases<lc::operation::Base>(),
            "new", [](int n) { return std::make_shared<lc::operation::Loop>(n); },
            "process", &lc::operation::Loop::process
            );

    operation.new_usertype<lc::operation::Begin>(
            "Begin",
            sol::base_classes, sol::bases<lc::operation::Base>(),
            "new", []() { return std::make_shared<lc::operation::Begin>(); },
            "getEntities", &lc::operation::Begin::getEntities,
            "process", &lc::operation::Begin::process
            );

    operation.new_usertype<lc::operation::Move>(
            "Move",
            sol::base_classes, sol::bases<lc::operation::Base>(),
            "new", [](lc::geo::Coordinate offset) { return std::make_shared<lc::operation::Move>(offset); },
            "process", &lc::operation::Move::process
            );

    operation.new_usertype<lc::operation::Copy>(
            "Copy",
            sol::base_classes, sol::bases<lc::operation::Base>(),
            "new", [](lc::geo::Coordinate offset) { return std::make_shared<lc::operation::Copy>(offset); },
            "process", &lc::operation::Copy::process
            );

    operation.new_usertype<lc::operation::Rotate>(
            "Rotate",
            sol::base_classes, sol::bases<lc::operation::Base>(),
            "new", [](lc::geo::Coordinate center, double angle) { return std::make_shared<lc::operation::Rotate>(center, angle); },
            "process", &lc::operation::Rotate::process
            );

    operation.new_usertype<lc::operation::Scale>(
            "Scale",
            sol::base_classes, sol::bases<lc::operation::Base>(),
            "new", [](lc::geo::Coordinate center, lc::geo::Coordinate factor) { return std::make_shared<lc::operation::Scale>(center, factor); },
            "process", &lc::operation::Scale::process
            );

    operation.new_usertype<lc::operation::Push>(
            "Push",
            sol::base_classes, sol::bases<lc::operation::Base>(),
            "new", []() { return std::make_shared<lc::operation::Push>(); },
            "process", &lc::operation::Push::process
            );

    operation.new_usertype<lc::operation::SelectByLayer>(
            "SelectByLayer",
            sol::base_classes, sol::bases<lc::operation::Base>(),
            "new", [](lc::meta::Layer_CSPtr layer) { return std::make_shared<lc::operation::SelectByLayer>(layer); },
            "process", &lc::operation::SelectByLayer::process
            );

    operation.new_usertype<lc::operation::Remove>(
            "Remove",
            sol::base_classes, sol::bases<lc::operation::Base>(),
            "new", []() { return std::make_shared<lc::operation::Remove>(); },
            "process", &lc::operation::Remove::process
            );

    operation.new_usertype<lc::operation::AddLayer>(
            "AddLayer",
            sol::base_classes, sol::bases<lc::operation::DocumentOperation>(),
            "new", [](lc::storage::Document_SPtr document, lc::meta::Layer_CSPtr layer) 
            { return std::make_shared<lc::operation::AddLayer>(document, layer); },
            "redo", &lc::operation::AddLayer::redo,
            "undo", &lc::operation::AddLayer::undo
            );

    operation.new_usertype<lc::operation::RemoveLayer>(
            "RemoveLayer",
            sol::base_classes, sol::bases<lc::operation::DocumentOperation>(),
            "new", [](lc::storage::Document_SPtr document, lc::meta::Layer_CSPtr layer) 
            { return std::make_shared<lc::operation::RemoveLayer>(document, layer); },
            "redo", &lc::operation::RemoveLayer::redo,
            "undo", &lc::operation::RemoveLayer::undo
            );

    operation.new_usertype<lc::operation::ReplaceLayer>(
            "ReplaceLayer",
            sol::base_classes, sol::bases<lc::operation::DocumentOperation>(),
            "new", [](lc::storage::Document_SPtr document, lc::meta::Layer_CSPtr layer, lc::meta::Layer_CSPtr newLayer) 
            { return std::make_shared<lc::operation::ReplaceLayer>(document, layer, newLayer); },
            "redo", &lc::operation::ReplaceLayer::redo,
            "undo", &lc::operation::ReplaceLayer::undo
            );

    operation.new_usertype<lc::operation::AddBlock>(
            "AddBlock",
            sol::base_classes, sol::bases<lc::operation::DocumentOperation>(),
            "new", [](lc::storage::Document_SPtr document, lc::meta::Block_CSPtr block) 
            { return std::make_shared<lc::operation::AddBlock>(document, block); },
            "redo", &lc::operation::AddBlock::redo,
            "undo", &lc::operation::AddBlock::undo
            );

    operation.new_usertype<lc::operation::RemoveBlock>(
            "RemoveBlock",
            sol::base_classes, sol::bases<lc::operation::DocumentOperation>(),
            "new", [](lc::storage::Document_SPtr document, lc::meta::Block_CSPtr block) 
            { return std::make_shared<lc::operation::RemoveBlock>(document, block); },
            "redo", &lc::operation::RemoveBlock::redo,
            "undo", &lc::operation::RemoveBlock::undo
            );

    operation.new_usertype<lc::operation::ReplaceBlock>(
            "ReplaceBlock",
            sol::base_classes, sol::bases<lc::operation::DocumentOperation>(),
            "new", [](lc::storage::Document_SPtr document, lc::meta::Block_CSPtr block, lc::meta::Block_CSPtr newBlock) 
            { return std::make_shared<lc::operation::ReplaceBlock>(document, block, newBlock); },
            "redo", &lc::operation::ReplaceBlock::redo,
            "undo", &lc::operation::ReplaceBlock::undo
            );

    operation.new_usertype<lc::operation::AddLinePattern>(
            "AddLinePattern",
            sol::base_classes, sol::bases<lc::operation::DocumentOperation>(),
            "new", [](lc::storage::Document_SPtr document, lc::meta::DxfLinePattern_CSPtr lp) 
            { return std::make_shared<lc::operation::AddLinePattern>(document, lp); },
            "redo", &lc::operation::AddLinePattern::redo,
            "undo", &lc::operation::AddLinePattern::undo
            );

    operation.new_usertype<lc::operation::RemoveLinePattern>(
            "RemoveLinePattern",
            sol::base_classes, sol::bases<lc::operation::DocumentOperation>(),
            "new", [](lc::storage::Document_SPtr document, lc::meta::DxfLinePattern_CSPtr lp) 
            { return std::make_shared<lc::operation::RemoveLinePattern>(document, lp); },
            "redo", &lc::operation::RemoveLinePattern::redo,
            "undo", &lc::operation::RemoveLinePattern::undo
            );

    operation.new_usertype<lc::operation::ReplaceLinePattern>(
            "ReplaceLinePattern",
            sol::base_classes, sol::bases<lc::operation::DocumentOperation>(),
            "new", [](lc::storage::Document_SPtr document, lc::meta::DxfLinePattern_CSPtr lp, lc::meta::DxfLinePattern_CSPtr newLP) 
            { return std::make_shared<lc::operation::ReplaceLinePattern>(document, lp, newLP); },
            "redo", &lc::operation::ReplaceLinePattern::redo,
            "undo", &lc::operation::ReplaceLinePattern::undo
            );

    operation.new_usertype<lc::operation::EntityBuilder>(
            "EntityBuilder",
            sol::base_classes, sol::bases<lc::operation::DocumentOperation>(),
            "new", [](const lc::storage::Document_SPtr& document) 
            { return std::make_shared<lc::operation::EntityBuilder>(document); },
            "appendEntity", &lc::operation::EntityBuilder::appendEntity,
            "appendOperation", &lc::operation::EntityBuilder::appendOperation,
            "processStack", &lc::operation::EntityBuilder::processStack,
            "redo", &lc::operation::EntityBuilder::redo,
            "undo", &lc::operation::EntityBuilder::undo
            );
}
