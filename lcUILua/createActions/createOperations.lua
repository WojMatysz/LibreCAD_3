CreateOperations = {}

setmetatable(CreateOperations, {
    __index = Operations
})

function CreateOperations:_init(builder, step)
    print("CreateOperations _init called, step=" .. tostring(step))
    Operations._init(self)

    self.prevEntity = nil
    if(builder ~= nil) then
        self.builder = builder()
    end

    self.step = step

    self:registerEvents()
end

function CreateOperations:onEvent(eventName, data)
    --print("CreateOperations: onEvent called")
    if(self.step == 'PAUSE') then
        return
    end

    if(self.step ~= nil) then
        self[self.step](self, eventName, data)
    end

    if(not self.finished) then
        self:refreshTempEntity()
    end
end

function CreateOperations:createEntity()
    print("CreateOperations: createEntity() called")
    print("EntityBuilder type: ", type(lc.operation.EntityBuilder))
    print("EntityBuilder is: ", tostring(lc.operation.EntityBuilder))

    local doc = mainWindow:cadMdiChild():document()
    print("document tostring:", tostring(doc), " type:", type(doc))

    local ok, res = pcall(function() return lc.operation.EntityBuilder(doc) end)
    print("pcall result:", ok, res)

    local b = lc.operation.EntityBuilder(mainWindow:cadMdiChild():document())
    print("EntityBuilder instance: ", b)
    print("CreateOperations: createEntity() called and entityBuilder should now exists")
    b:appendEntity(self:build())
    print("CreateOperations: createEntity() called and entity should be build")
    b:execute()

    self:manualClose()
end

function CreateOperations:build()
    self.builder:setLayer(mainWindow:cadMdiChild():activeLayer())
    self.builder:setMetaInfo(mainWindow:cadMdiChild():metaInfoManager():metaInfo())
    self.builder:setBlock(mainWindow:cadMdiChild():activeViewport())

    return self.builder:build()
end

function CreateOperations:refreshTempEntity()
    if (self.prevEntity ~= nil) then
        mainWindow:cadMdiChild():tempEntities():removeEntity(self.prevEntity)
    end

    self.entity = self:build()

    if (self.entity ~= nil) then
        mainWindow:cadMdiChild():tempEntities():addEntity(self.entity)
    end

    self.prevEntity = self.entity
end

function CreateOperations:removeTempEntity()
    if (self.prevEntity ~= nil) then
        mainWindow:cadMdiChild():tempEntities():removeEntity(self.prevEntity)
    end
end

function CreateOperations:unregisterEvents()
    print("CreateOperation: unregisterEvents called")
    luaInterface:deleteEvent('mouseMove', self)
    luaInterface:deleteEvent('number', self)
    luaInterface:deleteEvent('point', self)
    luaInterface:deleteEvent('text', self)
end

function CreateOperations:registerEvents()
    print("CreateOperation: registerEvents called")
    luaInterface:registerEvent('point', self)
    luaInterface:registerEvent('mouseMove', self)
    luaInterface:registerEvent('number', self)
    luaInterface:registerEvent('text', self)
end

function CreateOperations:close()
    if(not self.finished) then
        luaInterface:triggerEvent('operationFinished')
        self:removeTempEntity()
        if (self.cleanUp ~= nil) then
            self:cleanUp()
        end
        self:unregisterEvents()
        mainWindow:cliCommand():returnText(false)
        mainWindow:cliCommand():commandActive(false)
        self.finished = true
    end
end

function CreateOperations:manualClose()
    self:close() --if not defined manually
end

function CreateOperations:pauseCreation(pause)
    if(pause) then
        self.lastStep = self.step
        self.step = 'PAUSE'
    else
        if(self.lastStep == nil) then
            return
        end
        self.step = self.lastStep 
    end
end

function CreateOperations:isPaused()
    if(self.step == 'PAUSE') then
        return true
    else
        return false
    end
end
