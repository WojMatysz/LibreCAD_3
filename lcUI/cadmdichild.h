#pragma once
#include <QVBoxLayout>
#include <QWidget>
#include <QKeyEvent>
#include "lcadviewerproxy.h"
#include "cad/meta/color.h"
#include <cad/storage/storagemanager.h>

#include "cad/storage/document.h"
#include "cad/storage/undomanager.h"
#include <drawables/lccursor.h>

#include <file.h>
#include <managers/metainfomanager.h>

#include "sol.hpp"


namespace lc {
namespace ui {
class CadMdiChild : public QWidget {
    Q_OBJECT

public:
    explicit CadMdiChild(QWidget* parent = 0);

    ~CadMdiChild();

    /**
     * \brief Create a new document.
     */
    void newDocument();

    /**
     * \brief Load existing file.
     * \param path Path to file
     * \return bool True if the file was correctly opened, false otherwise.
     */
    bool openFile();

    /**
     * \brief Give function to call when window is destroyed
     * \param callback Lua function
     */
    void setDestroyCallback(sol::function destroyCallback);

    void keyPressEvent(QKeyEvent* event);
    lc::meta::Block_CSPtr activeViewport() const {
        return _viewerProxy->activeViewport();
    };
private://Add this as private
    LCADViewerProxy* viewerProxy() {
        return _viewerProxy;
    };

public slots:

    void ctxMenu(const QPoint& pos);
    void saveFile();
    void saveAsFile();

signals:

    void keyPressed(QKeyEvent* event);
    void keyPressEventx(int key); //temporary soln, need custom event handlers

signals:
    void mouseMoveEvent();
    void mousePressEvent();
    void mouseReleaseEvent();
    void selectionChangeEvent();

public:
    QWidget* view() const;

    std::shared_ptr<lc::storage::Document> document() const;

    lc::storage::UndoManager_SPtr undoManager() const;

    lc::viewer::manager::SnapManager_SPtr snapManager() const;

    lc::storage::StorageManager_SPtr storageManager() const;

    lc::ui::LCADViewer* viewer() {
        return _viewerProxy->viewer();
    }

    std::shared_ptr<lc::viewer::drawable::Cursor> cursor() const;

    /**
     * @brief Get the selected layer
     * @return Selected layer
     */
    lc::meta::Layer_CSPtr activeLayer() const;

    void setActiveLayer(const lc::meta::Layer_CSPtr& activeLayer);

    /**
     * @brief Get the MetaInfo manager
     * @return MetaInfoManager
     */
    lc::ui::MetaInfoManager_SPtr metaInfoManager() const;

    /**
     * \brief Get container of temporary entities
     * \return Temporary entities container
     */
    lc::viewer::drawable::TempEntities_SPtr tempEntities();

    /**
     * \brief Get selected entities
     * \return Selected entities
     * Return a vector of selected entities.
     * This function was added for Lua which can't access EntityContainer functions
     */
    std::vector<lc::entity::CADEntity_CSPtr> selection();

    const viewer::manager::SnapManagerImpl_SPtr getSnapManager() const;

    std::string getFilename() {
        return _filename;
    }

private:
    std::string _filename;
    lc::persistence::File::Type _fileType = lc::persistence::File::Type::LIBDXFRW_DXF_R2000;

    sol::function _destroyCallback;

    std::shared_ptr<lc::storage::Document> _document;

    storage::StorageManager_SPtr _storageManager;

    meta::Layer_CSPtr _activeLayer;
    ui::MetaInfoManager_SPtr _metaInfoManager;

    ui::LCADViewerProxy* _viewerProxy;
};
}
}
