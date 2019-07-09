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
local centralWidget = Widget.new(m_centralWidget)
local infoWidget = Widget.new()

local menuBar = Widget.new(m_menuBar)

local playlistView = Widget.new(m_playlistView)

local playButton = PushButton.new(m_playButton)
local stopButton = PushButton.new(m_stopButton)
local prevButton = PushButton.new(m_prevButton)
local nextButton = PushButton.new(m_nextButton)
local recordButton = PushButton.new(m_recordButton)
local openLinkButton = PushButton.new(m_openLinkButton)
local radioPlaylistButton = PushButton.new(m_radioPlaylistButton)
local changeFavoriteButton = PushButton.new(m_changeFavoriteButton)
local allPlaylistsModeButton = PushButton.new(m_allPlaylistsModeButton)
local customPlaylistModeButton = PushButton.new(m_customPlaylistModeButton)
local favoriteModeButton = PushButton.new(m_favoriteModeButton)

local timeLabel = Label.new(m_timeLabel)
local statusLabel = Label.new(m_statusLabel)
local nameLabel = Label.new(m_nameLabel)
local streamTitleLabel = Label.new(m_streamTitleLabel)

local loaderMovie = Widget.new(m_loaderMovie)

local leftChProgressBar = ProgressBar.new(m_leftChProgressBar)
local rightChProgressBar = ProgressBar.new(m_rightChProgressBar)
local bufferProgressBar = ProgressBar.new(m_bufferProgressBar)

local searchLineEdit = LineEdit.new(m_searchLineEdit)

local volumeControl = Widget.new(m_volumeControl)

-------------------------------------------------------------------------------
-- setObjectName() --
-------------------------------------------------------------------------------

mainWindow:setObjectName("window")
--titleBarWidget:setObjectName("")
--centralWidget:setObjectName("")
infoWidget:setObjectName("radioInfoWidget")

--menuBar:setObjectName("")

--playlistView:setObjectName("")

playButton:setObjectName("playButton")
stopButton:setObjectName("stopButton")
prevButton:setObjectName("prevButton")
nextButton:setObjectName("nextButton")
recordButton:setObjectName("recordButton")
openLinkButton:setObjectName("openLinkButton")
radioPlaylistButton:setObjectName("radioPlaylistButton")
changeFavoriteButton:setObjectName("changeFavoriteButton")
allPlaylistsModeButton:setObjectName("tabStyle")
customPlaylistModeButton:setObjectName("tabStyle")
favoriteModeButton:setObjectName("tabStyleLast")

timeLabel:setObjectName("timeLabel")
statusLabel:setObjectName("statusLabel")
nameLabel:setObjectName("radioTitleLabel")
streamTitleLabel:setObjectName("radioTitleLabel")

--loaderMovie:setObjectName("")

leftChProgressBar:setObjectName("channelsProgressBar")
rightChProgressBar:setObjectName("channelsProgressBar")
bufferProgressBar:setObjectName("bufferProgressBar")

searchLineEdit:setObjectName("searchLineEdit")

volumeControl:setObjectName("volumeButton")

-------------------------------------------------------------------------------
-- setToolTip() --
-------------------------------------------------------------------------------

mainWindow:setToolTip("")
titleBarWidget:setToolTip("")
centralWidget:setToolTip("")
infoWidget:setToolTip("")

menuBar:setToolTip("")

--playlistView:setToolTip("")

playButton:setToolTip("")
stopButton:setToolTip("")
prevButton:setToolTip("")
nextButton:setToolTip("")
recordButton:setToolTip("")
openLinkButton:setToolTip("Abrir Link Rapidamente")
radioPlaylistButton:setToolTip("Editar Lista de Rádios")
changeFavoriteButton:setToolTip("")
allPlaylistsModeButton:setToolTip("Todos")
customPlaylistModeButton:setToolTip("Personalizados")
favoriteModeButton:setToolTip("Favoritos")

timeLabel:setToolTip("")
statusLabel:setToolTip("")
nameLabel:setToolTip("")
streamTitleLabel:setToolTip("")

--loaderMovie:setToolTip("")

leftChProgressBar:setToolTip("")
rightChProgressBar:setToolTip("")
bufferProgressBar:setToolTip("Buffer")

searchLineEdit:setToolTip("")

volumeControl:setToolTip("")

-------------------------------------------------------------------------------
-- setText() --
-------------------------------------------------------------------------------

timeLabel:setText("--:--:--")
statusLabel:setText("---")

allPlaylistsModeButton:setText("Todos")
customPlaylistModeButton:setText("Personalizados")
favoriteModeButton:setText("Favoritos")

-------------------------------------------------------------------------------
-- setEnabled() --
-------------------------------------------------------------------------------

stopButton:setEnabled(false)

-------------------------------------------------------------------------------
-- setTextVisible() --
-------------------------------------------------------------------------------
    
leftChProgressBar:setTextVisible(false)
rightChProgressBar:setTextVisible(false)
bufferProgressBar:setTextVisible(false)

-------------------------------------------------------------------------------
-- setAlignment() --
-------------------------------------------------------------------------------

nameLabel:setAlignment(Alignment.AlignCenter)
streamTitleLabel:setAlignment(Alignment.AlignCenter)

-------------------------------------------------------------------------------
-- setOrientation() --
-------------------------------------------------------------------------------

leftChProgressBar:setOrientation(Orientation.Horizontal)
rightChProgressBar:setOrientation(Orientation.Horizontal)

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

local buttonLayout = BoxLayout.new(BoxLayout.Horizontal)
buttonLayout:setSpacing(7)
buttonLayout:addWidget(timeLabel)
buttonLayout:addSpacing(3)
buttonLayout:addWidget(changeFavoriteButton)
buttonLayout:addWidget(openLinkButton)
buttonLayout:addWidget(radioPlaylistButton)
buttonLayout:setAlignment(Alignment.AlignRight)

local topLayout = BoxLayout.new(BoxLayout.Horizontal)
topLayout:setSpacing(10)
topLayout:setContentsMargins(8, 0, 8, 8)
topLayout:addWidget(bufferProgressBar)
topLayout:addWidget(statusLabel, 1)
topLayout:addLayout(buttonLayout)

-----------------------------------------------------------

local searchLayout = BoxLayout.new(BoxLayout.Horizontal)
searchLayout:setSpacing(0)
searchLayout:setContentsMargins(8, 0, 8, 8)
searchLayout:addWidget(allPlaylistsModeButton)
searchLayout:addWidget(customPlaylistModeButton)
searchLayout:addWidget(favoriteModeButton)
searchLayout:addSpacing(8)
searchLayout:addWidget(searchLineEdit)

-----------------------------------------------------------

local playlistLayout = BoxLayout.new(BoxLayout.Vertical)
playlistLayout:setContentsMargins(8, 0, 8, 8)
playlistLayout:addWidget(playlistView)

-----------------------------------------------------------

local infoLayout = BoxLayout.new(BoxLayout.Vertical)
infoLayout:addWidget(nameLabel, 0, Alignment.AlignHCenter)
infoLayout:addWidget(streamTitleLabel, 0, Alignment.AlignHCenter)
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
bottomLayout:addWidget(stopButton)
bottomLayout:addWidget(prevButton)
bottomLayout:addWidget(nextButton)
bottomLayout:addWidget(recordButton)
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


