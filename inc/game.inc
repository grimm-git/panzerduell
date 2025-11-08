' Game objects
' Code by "Matthias Grimm"

CONST STATE_INTRO=0
CONST STATE_CONFIG=1
CONST STATE_DEMO=2
CONST STATE_GAME=3
CONST STATE_DEAD=4
CONST STATE_PLAYER=5
CONST STATE_LEVEL=6
CONST STATE_OVER=7
CONST STATE_CUTSCENE=8
CONST STATE_VICTORY=9

'Memory Management      8Bit                16Bit
CONST PAGE_DISPLAY=0   '$24000000-$2404AFFF $D0000000-$D0095FFF
CONST PAGE_TITLE=2     '                    $D012C000-$D01C1FFF
CONST PAGE_BUFFER=3    '$D0098000-$D00E2FFF $D01C2000-$D0257FFF
CONST PAGE_PLAYFIELD=9 '$D0260000-$D02AAFFF   assembled background
CONST PAGE_SPRITES=10  '$ 
CONST PAGE_TILESET=11  '$ 
CONST PAGE_BACKGND=12  '$D0344000-$D038EFFF   clean background
CONST PAGE_DEVICES=7   '                    $D041A000-$D04AFFFF


DIM Integer Game.State      ' Default=STATE_INTRO
DIM Integer Game.Ready
DIM Integer Game.Level      ' levels start with 1
DIM Integer Game.Requests   ' bitfield
DIM Integer Game.NumPlayers
DIM Float   Game.frameTime=1
DIM Float   Game.frameTimer

[DBG] DIM Float   DBG.updateTime

sub Game.init()
  Game.Ready=0

  Controls.init
  Settings.load("PanzerDuell.cfg")
  Panzer.init
  Shots.init
  Player.init
  Explosion.init
  Particle.init
  Drohne.init
  Map.init
end sub

sub Game.loadAssets()
  LOCAL Integer n
  
  mode 8,8
  load data "img/sprites.raw",MM.Info(page address PAGE_SPRITES)
  load data "img/playfield.raw",MM.Info(page address PAGE_BACKGND)
'  load data "img/grasland.raw",MM.Info(page address PAGE_BACKGND)
  load data "img/tileset.raw",MM.Info(page address PAGE_TILESET)
  if Not Map.loadMasks("img/tileset.masks") then
    text MM.HRES/2,MM.VRES/2,"Generating Game Data...","C",3,,map(8),-1
    for n=0 to NUMTILES-1: Map.Masks(n)=Map.calcTileMask(n):next
    Map.saveMasks("img/tileset.masks")
  endif

  cmap.init
  cmap.load "img/colors.cmap"

  mode 8,16
  page write PAGE_TITLE
  load png "img/title.png"
  page write PAGE_DEVICES
  load png "img/devices.png"
end sub

sub Game.new
  Game.State=STATE_INTRO
end sub

sub Game.start
  mode 8,8

  playMOD "soundfx"
  Particle.init
  Drohne.init

  cmap.activate
  page write PAGE_PLAYFIELD
  blit 0,0,0,0,640,480,PAGE_BACKGND,0
  Map.draw
  page write PAGE_BUFFER
  page display PAGE_DISPLAY

  Game.Ready=1
end sub

sub Game.clrScreen
  Game.frameTimer=TIMER
  if Game.State=STATE_INTRO or Game.STATE=STATE_CONFIG then
    cls
  else
    blit 0,0,0,0,640,480,PAGE_PLAYFIELD,0
    Game.drawDashboard
  endif
end sub

sub Game.update
  Particle.update
  Drohne.update
end sub

sub Game.draw
  STATIC Integer stage

  Shots.update
  Panzer.draw
  Explosion.draw
  Drohne.draw

  Game.getReady
end sub

sub Game.drawDashboard
  LOCAL Integer player,x,xpos(3)=(0,3,1,2)

  for player=1 to Game.NumPlayers
    x=xpos(player-1)
    text x*158+71,49,str$(player.getKills(player)),"R",3,,map(player.getCLUT(player)),-1
    text x*158+143,49,str$(player.getLives(player)),"R",3,,map(player.getCLUT(player)),-1
  next
end sub

sub Game.getReady
  STATIC Integer show=10
  STATIC Float tim

  if Game.Ready=1 AND TIMER-tim>400 then
    if show>0 then
      tim=TIMER
      inc show,-1
    else
      show=10
      Game.Ready=0
    endif
  endif
  if (show AND 1)=1 then
    text Screen.VPx+VP.W/2,Screen.VPy+VP.H/2-12,"Get Ready!","C",5,,map(130),-1
  endif
end sub

sub Game.swapPage
[DBG] DBG.updateTime=TIMER-Game.frameTimer
[DBG] Game.showFPS
  page copy PAGE_BUFFER to PAGE_DISPLAY,B
  Game.frameTime=TIMER-Game.frameTimer
end sub

Sub Game.showFPS
  text SCREEN.W-6,17,str$(int(1000/Game.frameTime)),"R",7,,,-1
  text SCREEN.W-6,27,str$(int(DBG.updateTime*10)/10,2,2)+"ms","R",7,,,-1
  text SCREEN.W-6,39,str$(cfr(1),2,2),"R",7,,,-1
end Sub


