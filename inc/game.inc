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

CONST PAGE_DISPLAY=0
CONST PAGE_BUFFER=1
CONST PAGE_BACK=2
CONST PAGE_SPRITES=3
CONST PAGE_TITLE=4


DIM Integer Game.State      ' Default=STATE_INTRO
DIM Integer Game.Ready
DIM Integer Game.Level      ' levels start with 1
DIM Integer Game.Requests   ' bitfield
DIM Float   Game.frameTimer
DIM Float   Game.gameTimer
DIM Integer Game.NumPlayers
DIM Float   Game.frameTime=1

DIM Float   DBG.updateTime


sub Game.init()
  Game.gameTimer=TIMER
  Game.Ready=0

  Controls.init
  Settings.load("PanzerDuell.cfg")
  Panzer.init
  Shots.init
  Player.init
  Explosion.init
  Particle.init
  Drohne.init
end sub

sub Game.loadAssets()
  load data "img/sprites.raw",MM.Info(page address PAGE_SPRITES)
  load data "img/playfield.raw",MM.Info(page address PAGE_BACK)
  cmap.init
  cmap.load "img/colors.cmap"
  cmap.activate
end sub

sub Game.new
  Game.State=STATE_INTRO
end sub

sub Game.start
  mode 8,8

  page display PAGE_DISPLAY
  page write PAGE_BUFFER
  Game.loadAssets
  playMOD "soundfx"
  Particle.init
  Game.Ready=1
end sub

sub Game.clrScreen
  Game.frameTimer=TIMER
  if Game.State=STATE_INTRO or Game.STATE=STATE_CONFIG then
    cls
  else
    blit 0,0,0,0,640,480,PAGE_BACK,0
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
[DBG] Game.frameTime=TIMER-Game.frameTimer
end sub

Sub Game.showFPS
  LOCAL Integer gameTime=TIMER-Game.gameTimer
  text SCREEN.W-6, 7,str$(int(gameTime/10))+"0","R",7,,,-1
  text SCREEN.W-6,17,str$(int(1000/Game.frameTime)),"R",7,,,-1
  text SCREEN.W-6,27,str$(int(DBG.updateTime*10)/10,2,2)+"ms","R",7,,,-1
  text SCREEN.W-6,39,str$(cfr(1),2,2),"R",7,,,-1
end Sub


