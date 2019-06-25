
-------------------------------------------------------------------------------
-- new() --
-------------------------------------------------------------------------------

local windowIconLabel = Label.new()
local windowTitleLabel = Label.new()
local previewLabel = Label.new("[3.0-RC1]")

local minimizeButton = PushButton.new()
local minimizeTrayButton = PushButton.new()
local closeButton = PushButton.new()

-------------------------------------------------------------------------------
-- setObjectName() --
-------------------------------------------------------------------------------

titleBarWidget:setObjectName("titleBar")

windowIconLabel:setObjectName("windowIcon")
windowTitleLabel:setObjectName("windowTitle")

minimizeButton:setObjectName("minimizeButton")
minimizeTrayButton:setObjectName("minimizeTrayButton")
closeButton:setObjectName("closeButton")

-------------------------------------------------------------------------------
-- setToolTip() --
-------------------------------------------------------------------------------

windowIconLabel:setToolTip("<3")
--windowTitleLabel:setToolTip("")

minimizeButton:setToolTip("Minimizar")
minimizeTrayButton:setToolTip("Minimizar para a Bandeja")
closeButton:setToolTip("Fechar")

-------------------------------------------------------------------------------
-- setText() --
-------------------------------------------------------------------------------

windowTitleLabel:setText(mainWindow:windowTitle())

-------------------------------------------------------------------------------
-- setStyleSheet() --
-------------------------------------------------------------------------------

previewLabel:setStyleSheet("QLabel { background: none; }")

-------------------------------------------------------------------------------
-- connect() --
-------------------------------------------------------------------------------

Object.connect(mainWindow, "windowTitleChanged(QString)", windowTitleLabel, "setText(QString)")
Object.connect(minimizeButton, "clicked()", mainWindow, "showMinimized()")
Object.connect(minimizeTrayButton, "clicked()", mainWindow, "hide()")
Object.connect(closeButton, "clicked()", titleBarWidget, "closeApp()")

-------------------------------------------------------------------------------
-- BoxLayout --
-------------------------------------------------------------------------------

local titleBarLayout = BoxLayout.new(BoxLayout.Horizontal)
titleBarLayout:setSpacing(0)
titleBarLayout:setMargin(0)
titleBarLayout:addWidget(windowIconLabel)
titleBarLayout:addWidget(windowTitleLabel)
titleBarLayout:addWidget(previewLabel, 1)
titleBarLayout:addWidget(minimizeButton)
titleBarLayout:addWidget(minimizeTrayButton)
titleBarLayout:addWidget(closeButton)

titleBarWidget:setLayout(titleBarLayout)
