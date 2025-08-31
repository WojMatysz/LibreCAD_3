#include "lc_storage.h"

#include <cad/base/cadentity.h>
#include <cad/storage/undomanager.h>
#include <cad/storage/undomanagerimpl.h>
#include <cad/storage/storagemanagerimpl.h>
#include <cad/storage/document.h>
#include <cad/storage/storagemanager.h>
#include <cad/storage/documentimpl.h>
#include <cad/primitive/insert.h>

void import_lc_storage_namespace(sol::state & luaVM) {
    sol::table lc = luaVM["lc"];
    lc["storage"] = luaVM.create_table();
    sol::table storage = lc["storage"];

    storage.new_usertype<lc::storage::QuadTreeSub<lc::entity::CADEntity_CSPtr>>(
            "QuadTreeSub",
            sol::constructors<
            lc::storage::QuadTreeSub<lc::entity::CADEntity_CSPtr>(int, const lc::geo::Area &, short, short), 
            lc::storage::QuadTreeSub<lc::entity::CADEntity_CSPtr>(const lc::geo::Area &), 
            lc::storage::QuadTreeSub<lc::entity::CADEntity_CSPtr>(const lc::storage::QuadTreeSub<lc::entity::CADEntity_CSPtr> &), 
            lc::storage::QuadTreeSub<lc::entity::CADEntity_CSPtr>()
            >(),
            "bounds", &lc::storage::QuadTreeSub<lc::entity::CADEntity_CSPtr>::bounds,
            "clear", &lc::storage::QuadTreeSub<lc::entity::CADEntity_CSPtr>::clear,
            "entityByID", &lc::storage::QuadTreeSub<lc::entity::CADEntity_CSPtr>::entityByID,
            "erase", &lc::storage::QuadTreeSub<lc::entity::CADEntity_CSPtr>::erase,
            "insert", sol::overload(
                static_cast<void(lc::storage::QuadTreeSub<lc::entity::CADEntity_CSPtr>::*)(const std::shared_ptr<const class lc::entity::CADEntity>, const lc::geo::Area &)>
                (&lc::storage::QuadTreeSub<lc::entity::CADEntity_CSPtr>::insert), 
                static_cast<void(lc::storage::QuadTreeSub<lc::entity::CADEntity_CSPtr>::*)(const std::shared_ptr<const class lc::entity::CADEntity>)>
                (&lc::storage::QuadTreeSub<lc::entity::CADEntity_CSPtr>::insert)
                ),
            "level", &lc::storage::QuadTreeSub<lc::entity::CADEntity_CSPtr>::level,
            "maxLevels", &lc::storage::QuadTreeSub<lc::entity::CADEntity_CSPtr>::maxLevels,
            "maxObjects", &lc::storage::QuadTreeSub<lc::entity::CADEntity_CSPtr>::maxObjects,
            "optimise", &lc::storage::QuadTreeSub<lc::entity::CADEntity_CSPtr>::optimise,
            "retrieve", sol::overload(
                static_cast<std::vector<lc::entity::CADEntity_CSPtr>(lc::storage::QuadTreeSub<lc::entity::CADEntity_CSPtr>::*)(const lc::geo::Area &, const short) const>
                (&lc::storage::QuadTreeSub<lc::entity::CADEntity_CSPtr>::retrieve), 
                static_cast<std::vector<lc::entity::CADEntity_CSPtr>(lc::storage::QuadTreeSub<lc::entity::CADEntity_CSPtr>::*)(const short) const>
                (&lc::storage::QuadTreeSub<lc::entity::CADEntity_CSPtr>::retrieve)
                ),
            "size", &lc::storage::QuadTreeSub<lc::entity::CADEntity_CSPtr>::size,
            "walkQuad", &lc::storage::QuadTreeSub<lc::entity::CADEntity_CSPtr>::walkQuad
                );

    storage.new_usertype<lc::storage::QuadTree<lc::entity::CADEntity_CSPtr>>(
            "QuadTree",
            sol::constructors<
            lc::storage::QuadTree<lc::entity::CADEntity_CSPtr>(int, const lc::geo::Area &, short, short),
            lc::storage::QuadTree<lc::entity::CADEntity_CSPtr>(const lc::geo::Area &),
            lc::storage::QuadTree<lc::entity::CADEntity_CSPtr>(const lc::storage::QuadTree<lc::entity::CADEntity_CSPtr> &), 
            lc::storage::QuadTree<lc::entity::CADEntity_CSPtr>()
            >(),
            sol::base_classes, sol::bases<lc::storage::QuadTreeSub<lc::entity::CADEntity_CSPtr>>(),
            "clear", &lc::storage::QuadTree<lc::entity::CADEntity_CSPtr>::clear,
            "entityByID", &lc::storage::QuadTree<lc::entity::CADEntity_CSPtr>::entityByID,
            "erase", &lc::storage::QuadTree<lc::entity::CADEntity_CSPtr>::erase,
            "insert", &lc::storage::QuadTree<lc::entity::CADEntity_CSPtr>::insert,
            "test", &lc::storage::QuadTree<lc::entity::CADEntity_CSPtr>::test
            );

    storage.new_usertype<lc::storage::EntityContainer<lc::entity::CADEntity_CSPtr>>(
            "EntityContainer",
            sol::constructors<
            lc::storage::EntityContainer<lc::entity::CADEntity_CSPtr>(), 
            lc::storage::EntityContainer<lc::entity::CADEntity_CSPtr>(const lc::storage::EntityContainer<lc::entity::CADEntity_CSPtr> &)
            >(),
            "asVector", &lc::storage::EntityContainer<lc::entity::CADEntity_CSPtr>::asVector,
            "boundingBox", &lc::storage::EntityContainer<lc::entity::CADEntity_CSPtr>::boundingBox,
            "bounds", &lc::storage::EntityContainer<lc::entity::CADEntity_CSPtr>::bounds,
            "combine", &lc::storage::EntityContainer<lc::entity::CADEntity_CSPtr>::combine,
            "entitiesByLayer", &lc::storage::EntityContainer<lc::entity::CADEntity_CSPtr>::entitiesByLayer,
            "entitiesByMetaType", &lc::storage::EntityContainer<lc::entity::CADEntity_CSPtr>::entitiesByMetaType,
            "entitiesFullWithinArea", &lc::storage::EntityContainer<lc::entity::CADEntity_CSPtr>::entitiesFullWithinArea,
            "entitiesWithinAndCrossingArea", &lc::storage::EntityContainer<lc::entity::CADEntity_CSPtr>::entitiesWithinAndCrossingArea,
            "entitiesWithinAndCrossingAreaFast", &lc::storage::EntityContainer<lc::entity::CADEntity_CSPtr>::entitiesWithinAndCrossingAreaFast,
            "entityByID", &lc::storage::EntityContainer<lc::entity::CADEntity_CSPtr>::entityByID,
            "getEntityPathsNearCoordinate", &lc::storage::EntityContainer<lc::entity::CADEntity_CSPtr>::getEntityPathsNearCoordinate,
            "insert", &lc::storage::EntityContainer<lc::entity::CADEntity_CSPtr>::insert,
            "optimise", &lc::storage::EntityContainer<lc::entity::CADEntity_CSPtr>::optimise,
            "remove", &lc::storage::EntityContainer<lc::entity::CADEntity_CSPtr>::remove
            );

    storage.new_usertype<lc::storage::StorageManager>(
            "StorageManager",
            "addDocumentMetaType", &lc::storage::StorageManager::addDocumentMetaType,
            "allLayers", &lc::storage::StorageManager::allLayers,
            "allMetaTypes", &lc::storage::StorageManager::allMetaTypes,
            "entitiesByBlock", &lc::storage::StorageManager::entitiesByBlock,
            "entitiesByLayer", &lc::storage::StorageManager::entitiesByLayer,
            "entityByID", &lc::storage::StorageManager::entityByID,
            "entityContainer", &lc::storage::StorageManager::entityContainer,
            "insertEntity", &lc::storage::StorageManager::insertEntity,
            "insertEntityContainer", &lc::storage::StorageManager::insertEntityContainer,
            "layerByName", &lc::storage::StorageManager::layerByName,
            "linePatternByName", &lc::storage::StorageManager::linePatternByName,
            "optimise", &lc::storage::StorageManager::optimise,
            "removeDocumentMetaType", &lc::storage::StorageManager::removeDocumentMetaType,
            "removeEntity", &lc::storage::StorageManager::removeEntity,
            "replaceDocumentMetaType", &lc::storage::StorageManager::replaceDocumentMetaType
            );


    storage.new_usertype<lc::storage::Document>(
            "Document",
            "addDocumentMetaType", &lc::storage::Document::addDocumentMetaType,
            "allLayers", &lc::storage::Document::allLayers,
            "allMetaTypes", &lc::storage::Document::allMetaTypes,
            "blocks", &lc::storage::Document::blocks,
            "entitiesByBlock", &lc::storage::Document::entitiesByBlock,
            "entitiesByLayer", &lc::storage::Document::entitiesByLayer,
            "entityContainer", &lc::storage::Document::entityContainer,
            "insertEntity", &lc::storage::Document::insertEntity,
            "layerByName", &lc::storage::Document::layerByName,
            "linePatternByName", &lc::storage::Document::linePatternByName,
            "linePatterns", &lc::storage::Document::linePatterns,
            "removeDocumentMetaType", &lc::storage::Document::removeDocumentMetaType,
            "removeEntity", &lc::storage::Document::removeEntity,
            "replaceDocumentMetaType", &lc::storage::Document::replaceDocumentMetaType,
            "waitingCustomEntities", &lc::storage::Document::waitingCustomEntities
            );

    storage.new_usertype<lc::storage::DocumentImpl>(
            "DocumentImpl",
            sol::constructors<lc::storage::DocumentImpl(lc::storage::StorageManager_SPtr)>(),
            sol::base_classes, sol::bases<lc::storage::Document>(),
            "addDocumentMetaType", &lc::storage::DocumentImpl::addDocumentMetaType,
            "allLayers", &lc::storage::DocumentImpl::allLayers,
            "allMetaTypes", &lc::storage::DocumentImpl::allMetaTypes,
            "blocks", &lc::storage::DocumentImpl::blocks,
            "entitiesByBlock", &lc::storage::DocumentImpl::entitiesByBlock,
            "entitiesByLayer", &lc::storage::DocumentImpl::entitiesByLayer,
            "entityContainer", &lc::storage::DocumentImpl::entityContainer,
            "insertEntity", &lc::storage::DocumentImpl::insertEntity,
            "layerByName", &lc::storage::DocumentImpl::layerByName,
            "linePatternByName", &lc::storage::DocumentImpl::linePatternByName,
            "linePatterns", &lc::storage::DocumentImpl::linePatterns,
            "removeDocumentMetaType", &lc::storage::DocumentImpl::removeDocumentMetaType,
            "removeEntity", &lc::storage::DocumentImpl::removeEntity,
            "replaceDocumentMetaType", &lc::storage::DocumentImpl::replaceDocumentMetaType,
            "waitingCustomEntities", &lc::storage::DocumentImpl::waitingCustomEntities
            );

    storage.new_usertype<lc::storage::UndoManager>(
            "UndoManager",
            "canRedo", &lc::storage::UndoManager::canRedo,
            "canUndo", &lc::storage::UndoManager::canUndo,
            "redo", &lc::storage::UndoManager::redo,
            "removeUndoables", &lc::storage::UndoManager::removeUndoables,
            "undo", &lc::storage::UndoManager::undo
            );

    storage.new_usertype<lc::storage::UndoManagerImpl>(
            "UndoManagerImpl",
            sol::constructors<lc::storage::UndoManagerImpl(unsigned int)>(),
            sol::base_classes, sol::bases<lc::storage::UndoManager>(),
            "canRedo", &lc::storage::UndoManagerImpl::canRedo,
            "canUndo", &lc::storage::UndoManagerImpl::canUndo,
            "on_CommitProcessEvent", &lc::storage::UndoManagerImpl::on_CommitProcessEvent,
            "redo", &lc::storage::UndoManagerImpl::redo,
            "removeUndoables", &lc::storage::UndoManagerImpl::removeUndoables,
            "undo", &lc::storage::UndoManagerImpl::undo
            );

    storage.new_usertype<lc::storage::StorageManagerImpl>(
            "StorageManagerImpl",
            sol::constructors<lc::storage::StorageManagerImpl()>(),
            sol::base_classes, sol::bases<lc::storage::StorageManager>(),
            "addDocumentMetaType", &lc::storage::StorageManagerImpl::addDocumentMetaType,
            "allLayers", &lc::storage::StorageManagerImpl::allLayers,
            "allMetaTypes", &lc::storage::StorageManagerImpl::allMetaTypes,
            "entitiesByBlock", &lc::storage::StorageManagerImpl::entitiesByBlock,
            "entitiesByLayer", &lc::storage::StorageManagerImpl::entitiesByLayer,
            "entityByID", &lc::storage::StorageManagerImpl::entityByID,
            "entityContainer", &lc::storage::StorageManagerImpl::entityContainer,
            "insertEntity", &lc::storage::StorageManagerImpl::insertEntity,
            "insertEntityContainer", &lc::storage::StorageManagerImpl::insertEntityContainer,
            "layerByName", &lc::storage::StorageManagerImpl::layerByName,
            "linePatternByName", &lc::storage::StorageManagerImpl::linePatternByName,
            "optimise", &lc::storage::StorageManagerImpl::optimise,
            "removeDocumentMetaType", &lc::storage::StorageManagerImpl::removeDocumentMetaType,
            "removeEntity", &lc::storage::StorageManagerImpl::removeEntity,
            "replaceDocumentMetaType", &lc::storage::StorageManagerImpl::replaceDocumentMetaType
            );
}
