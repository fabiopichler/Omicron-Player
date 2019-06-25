
-------------------------------------------------------------------------------
-- new() --
-------------------------------------------------------------------------------

mainWindow = Widget.new(m_mainWindow)--
titleBarWidget = Widget.new(m_titleBar)--
local centralWidget = Widget.new(m_centralWidget)--
local statusWidget = Widget.new()

local menuBar = Widget.new(m_menuBar)--

local playlistView = Widget.new(m_playlistView)--

local playButton = PushButton.new(m_playButton)--
local stopButton = PushButton.new(m_stopButton)--
local recordButton = PushButton.new(m_recordButton)--

local timeLabel = Label.new(m_timeLabel)--
local statusLabel = Label.new(m_statusLabel)--

local leftChProgressBar = ProgressBar.new(m_leftChProgressBar)--
local rightChProgressBar = ProgressBar.new(m_rightChProgressBar)--

local timeSlider = Slider.new(m_timeSlider)
local deviceComboBox = ComboBox.new(m_deviceComboBox)
local encoderComboBox = ComboBox.new(m_encoderComboBox)
local bitrateComboBox = ComboBox.new(m_bitrateComboBox)
local deleteButton = PushButton.new(m_deleteButton)

-------------------------------------------------------------------------------
-- setObjectName() --
-------------------------------------------------------------------------------

mainWindow:setObjectName("window")
--titleBarWidget:setObjectName("")
--centralWidget:setObjectName("")
statusWidget:setObjectName("statusWidget")

--menuBar:setObjectName("")

--playlistView:setObjectName("")

playButton:setObjectName("playButton")
stopButton:setObjectName("stopButton")
recordButton:setObjectName("recordButton")

timeLabel:setObjectName("timeLabel")
statusLabel:setObjectName("statusLabel")

leftChProgressBar:setObjectName("channelsProgressBar")
rightChProgressBar:setObjectName("channelsProgressBar")

timeSlider:setObjectName("timeSlider")

-------------------------------------------------------------------------------
-- setToolTip() --
-------------------------------------------------------------------------------

mainWindow:setToolTip("")
titleBarWidget:setToolTip("")
centralWidget:setToolTip("")
statusWidget:setToolTip("")

menuBar:setToolTip("")

--playlistView:setToolTip("")

playButton:setToolTip("")
stopButton:setToolTip("")
recordButton:setToolTip("")

timeLabel:setToolTip("")
statusLabel:setToolTip("")

leftChProgressBar:setToolTip("")
rightChProgressBar:setToolTip("")

-------------------------------------------------------------------------------
-- setText() --
-------------------------------------------------------------------------------

timeLabel:setText("--:--:--")
statusLabel:setText("KKKK---")

-------------------------------------------------------------------------------
-- setEnabled() --
-------------------------------------------------------------------------------

timeSlider:setEnabled(false)
stopButton:setEnabled(false)

-------------------------------------------------------------------------------
-- setTextVisible() --
-------------------------------------------------------------------------------
    
leftChProgressBar:setTextVisible(false)
rightChProgressBar:setTextVisible(false)

-------------------------------------------------------------------------------
-- setAlignment() --
-------------------------------------------------------------------------------

statusLabel:setAlignment(Alignment.AlignCenter)

-------------------------------------------------------------------------------
-- setOrientation() --
-------------------------------------------------------------------------------

leftChProgressBar:setOrientation(Orientation.Horizontal)
rightChProgressBar:setOrientation(Orientation.Horizontal)

timeSlider:setOrientation(Orientation.Horizontal)

-------------------------------------------------------------------------------
-- setInvertedAppearance() --
-------------------------------------------------------------------------------

leftChProgressBar:setInvertedAppearance(true)

-------------------------------------------------------------------------------
-- setPlaceholderText() --
-------------------------------------------------------------------------------


-------------------------------------------------------------------------------
-- setMaximumHeight() --
-------------------------------------------------------------------------------

statusWidget:setMaximumHeight(20)

-------------------------------------------------------------------------------
-- setMaximumHeight() --
-------------------------------------------------------------------------------

statusLabel:setMinimumWidth(110)

-------------------------------------------------------------------------------
-- require --
-------------------------------------------------------------------------------

require "TitleBar"

-------------------------------------------------------------------------------
-- BoxLayout --
-------------------------------------------------------------------------------

local menuLayout = BoxLayout.new(BoxLayout.Vertical)
menuLayout:setContentsMargins(0, 0, 0, 8)
menuLayout:addWidget(menuBar)

-----------------------------------------------------------

local statusLayout = BoxLayout.new(BoxLayout.Horizontal)
statusLayout:setSpacing(0)
statusLayout:setMargin(0)
statusLayout:addWidget(statusLabel, 1)

statusWidget:setLayout(statusLayout)

-----------------------------------------------------------

local topLayout = BoxLayout.new(BoxLayout.Horizontal)
topLayout:setSpacing(8)
topLayout:setContentsMargins(8, 0, 8, 8)
topLayout:addWidget(deviceComboBox)
topLayout:addWidget(encoderComboBox)
topLayout:addWidget(bitrateComboBox)
topLayout:addWidget(deleteButton)

-----------------------------------------------------------

local playlistLayout = BoxLayout.new(BoxLayout.Vertical)
playlistLayout:setContentsMargins(8, 0, 8, 8)
playlistLayout:addWidget(playlistView)

-----------------------------------------------------------

local infoLayout = BoxLayout.new(BoxLayout.Horizontal)
infoLayout:setSpacing(8)
infoLayout:setContentsMargins(8, 0, 8, 0)
infoLayout:addWidget(timeLabel)
infoLayout:addWidget(timeSlider)

-----------------------------------------------------------

local channelLayout = BoxLayout.new(BoxLayout.Horizontal)
channelLayout:setSpacing(1)
channelLayout:addWidget(leftChProgressBar)
channelLayout:addWidget(rightChProgressBar)

-----------------------------------------------------------

local bottomLayout = BoxLayout.new(BoxLayout.Horizontal)
bottomLayout:setSpacing(5)
bottomLayout:setContentsMargins(8, 8, 8, 8)
bottomLayout:addWidget(recordButton)
bottomLayout:addWidget(stopButton)
bottomLayout:addWidget(playButton)
bottomLayout:addLayout(channelLayout, 1)
bottomLayout:addWidget(statusWidget)

-----------------------------------------------------------

local mainLayout = BoxLayout.new(BoxLayout.Vertical)
mainLayout:setSpacing(0)
mainLayout:setMargin(0)
mainLayout:addWidget(titleBarWidget)
mainLayout:addLayout(menuLayout)
mainLayout:addLayout(topLayout)
mainLayout:addLayout(playlistLayout)
mainLayout:addLayout(infoLayout)
mainLayout:addLayout(bottomLayout)

centralWidget:setLayout(mainLayout)
