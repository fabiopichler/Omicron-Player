--[[ *******************************************************************************

  Omicron Media Player

  Author: Fábio Pichler
  Website: http://fabiopichler.net
  License: BSD 3-Clause License

  Copyright (c) 2015-2019, Fábio Pichler
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

  * Neither the name of Omicron Media Player nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

******************************************************************************* --]]

-------------------------------------------------------------------------------
-- new() --
-------------------------------------------------------------------------------

mainWindow = Widget.new(m_mainWindow)
titleBarWidget = Widget.new(m_titleBar)
local playlistView = Widget.new(m_playlistView)
local centralWidget = Widget.new(m_centralWidget)
local tracksWidget = Widget.new()
local infoWidget = Widget.new()

local menuBar = Widget.new(m_menuBar)

local currentTrackLabel = Label.new(m_currentTrackLabel)
local totalTracksLabel = Label.new(m_totalTracksLabel)
local currentTagLabel = Label.new(m_currentTagLabel)
local totalTimeLabel = Label.new(m_totalTimeLabel)
local fileTypeLabel = Label.new(m_fileTypeLabel)
local timeLabel = Label.new(m_timeLabel)
local separatorLabel = Label.new("/")

local playButton = PushButton.new(m_playButton)
local pauseButton = PushButton.new(m_pauseButton)
local stopButton = PushButton.new(m_stopButton)
local prevButton = PushButton.new(m_prevButton)
local nextButton = PushButton.new(m_nextButton)
local changeFavoriteButton = PushButton.new(m_changeFavoriteButton)
local repeatButton = PushButton.new(m_repeatButton)
local randomButton = PushButton.new(m_randomButton)
local playlistManagerButton = PushButton.new(m_playlistManagerButton)
local musicTabButton = PushButton.new(m_musicTabButton)
local playlistTabButton = PushButton.new(m_playlistTabButton)
local favoritesTabButton = PushButton.new(m_favoritesTabButton)
local volumeControl = PushButton.new(m_volumeControl)

local timeSlider = Slider.new(m_timeSlider)

local leftChProgressBar = ProgressBar.new(m_leftChProgressBar)
local rightChProgressBar = ProgressBar.new(m_rightChProgressBar)

local searchLineEdit = LineEdit.new(m_searchLineEdit)

-------------------------------------------------------------------------------
-- setObjectName() --
-------------------------------------------------------------------------------

mainWindow:setObjectName("window")
--titleBarWidget:setObjectName("")
--playlistView:setObjectName("")
--centralWidget:setObjectName("")
tracksWidget:setObjectName("tracksWidget")
infoWidget:setObjectName("radioInfoWidget")

--menuBar:setObjectName("")

currentTrackLabel:setObjectName("trackLabel")
totalTracksLabel:setObjectName("trackLabel")
currentTagLabel:setObjectName("radioTitleLabel")
totalTimeLabel:setObjectName("radioTitleLabel")
fileTypeLabel:setObjectName("")
timeLabel:setObjectName("radioTitleLabel")
separatorLabel:setObjectName("trackLabel")

playButton:setObjectName("playButton")
pauseButton:setObjectName("pauseButton")
stopButton:setObjectName("stopButton")
prevButton:setObjectName("prevButton")
nextButton:setObjectName("nextButton")
changeFavoriteButton:setObjectName("changeFavoriteButton")
repeatButton:setObjectName("repeatButton")
randomButton:setObjectName("randomButton")
playlistManagerButton:setObjectName("playlistButton")
musicTabButton:setObjectName("tabStyle")
playlistTabButton:setObjectName("tabStyle")
favoritesTabButton:setObjectName("tabStyleLast")
volumeControl:setObjectName("volumeButton")

timeSlider:setObjectName("timeSlider")

leftChProgressBar:setObjectName("channelsProgressBar")
rightChProgressBar:setObjectName("channelsProgressBar")

searchLineEdit:setObjectName("searchLineEdit")

-------------------------------------------------------------------------------
-- setToolTip() --
-------------------------------------------------------------------------------

mainWindow:setToolTip("")
titleBarWidget:setToolTip("")
--playlistView:setToolTip("")
centralWidget:setToolTip("")
tracksWidget:setToolTip("")
infoWidget:setToolTip("")

menuBar:setToolTip("")

currentTrackLabel:setToolTip("Faixa atual")
totalTracksLabel:setToolTip("Total de faixas")
currentTagLabel:setToolTip("")
totalTimeLabel:setToolTip("")
fileTypeLabel:setToolTip("")
timeLabel:setToolTip("")
separatorLabel:setToolTip("")

playButton:setToolTip("Reproduzir")
pauseButton:setToolTip("Pausar")
stopButton:setToolTip("Parar")
prevButton:setToolTip("Faixa anterior/Retroceder")
nextButton:setToolTip("Próxima faixa")
changeFavoriteButton:setToolTip("")
repeatButton:setToolTip("")
randomButton:setToolTip("")
playlistManagerButton:setToolTip("Editar Playlist")
musicTabButton:setToolTip("")
playlistTabButton:setToolTip("")
favoritesTabButton:setToolTip("")
volumeControl:setToolTip("")

timeSlider:setToolTip("")

leftChProgressBar:setToolTip("")
rightChProgressBar:setToolTip("")

searchLineEdit:setToolTip("")

-------------------------------------------------------------------------------
-- setText() --
-------------------------------------------------------------------------------

fileTypeLabel:setText("---")
timeLabel:setText("--:--")
totalTimeLabel:setText("--:--")

-------------------------------------------------------------------------------
-- setEnabled() --
-------------------------------------------------------------------------------

timeSlider:setEnabled(false)
pauseButton:setEnabled(false)
stopButton:setEnabled(false)

-------------------------------------------------------------------------------
-- setTextVisible() --
-------------------------------------------------------------------------------
    
leftChProgressBar:setTextVisible(false)
rightChProgressBar:setTextVisible(false)

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

searchLineEdit:setPlaceholderText("Pesquisa rápida")

-------------------------------------------------------------------------------
-- setClearButtonEnabled() --
-------------------------------------------------------------------------------

searchLineEdit:setClearButtonEnabled(true)

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

local labelLayout = BoxLayout.new(BoxLayout.Horizontal)
labelLayout:setSpacing(6)
labelLayout:setContentsMargins(10, 0, 10, 0)
labelLayout:addWidget(currentTrackLabel)
labelLayout:addWidget(separatorLabel)
labelLayout:addWidget(totalTracksLabel)

tracksWidget:setLayout(labelLayout)

local topLayout = BoxLayout.new(BoxLayout.Horizontal)
topLayout:setSpacing(10)
topLayout:setContentsMargins(8, 0, 8, 8)
topLayout:addWidget(tracksWidget)
topLayout:addWidget(fileTypeLabel, 1)
topLayout:addWidget(changeFavoriteButton)
topLayout:addWidget(repeatButton)
topLayout:addWidget(randomButton)
topLayout:addWidget(playlistManagerButton)

-----------------------------------------------------------

local searchLayout = BoxLayout.new(BoxLayout.Horizontal)
searchLayout:setSpacing(0)
searchLayout:setContentsMargins(8, 0, 8, 8)
searchLayout:addWidget(musicTabButton)
searchLayout:addWidget(playlistTabButton)
searchLayout:addWidget(favoritesTabButton)
searchLayout:addSpacing(8)
searchLayout:addWidget(searchLineEdit)

-----------------------------------------------------------

local playlistLayout = BoxLayout.new(BoxLayout.Vertical)
playlistLayout:setContentsMargins(8, 0, 8, 8)
playlistLayout:addWidget(playlistView)

-----------------------------------------------------------

local timeLayout = BoxLayout.new(BoxLayout.Horizontal)
timeLayout:setSpacing(8)
timeLayout:setContentsMargins(0, 0, 0, 0)
timeLayout:addWidget(timeLabel)
timeLayout:addWidget(timeSlider)
timeLayout:addWidget(totalTimeLabel)

local infoLayout = BoxLayout.new(BoxLayout.Vertical)
infoLayout:setSpacing(0)
infoLayout:setContentsMargins(8, 7, 8, 7)
infoLayout:addWidget(currentTagLabel, 0, Alignment.AlignHCenter)
infoLayout:addLayout(timeLayout)

infoWidget:setLayout(infoLayout)

-----------------------------------------------------------

local channelLayout = BoxLayout.new(BoxLayout.Horizontal)
channelLayout:setSpacing(1)
channelLayout:addWidget(leftChProgressBar)
channelLayout:addWidget(rightChProgressBar)

local bottomLayout = BoxLayout.new(BoxLayout.Horizontal)
bottomLayout:setSpacing(5)
bottomLayout:setContentsMargins(8, 8, 8, 8)
bottomLayout:addWidget(playButton)
bottomLayout:addWidget(pauseButton)
bottomLayout:addWidget(stopButton)
bottomLayout:addWidget(prevButton)
bottomLayout:addWidget(nextButton)
bottomLayout:addLayout(channelLayout)
bottomLayout:addWidget(volumeControl)

-----------------------------------------------------------

local mainLayout = BoxLayout.new(BoxLayout.Vertical)
mainLayout:setSpacing(0)
mainLayout:setMargin(0)
mainLayout:addWidget(titleBarWidget)
mainLayout:addLayout(menuLayout)
mainLayout:addLayout(topLayout)
mainLayout:addLayout(searchLayout)
mainLayout:addLayout(playlistLayout)
mainLayout:addWidget(infoWidget)
mainLayout:addLayout(bottomLayout)

centralWidget:setLayout(mainLayout)


