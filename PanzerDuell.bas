
OPTION BASE 0
OPTION EXPLICIT
OPTION DEFAULT NONE

#DEFINE "[NUN]","'"  'enable extra Nunchuk functions, requires nunchuk.inc
#DEFINE "[DBG]","'"  'enable standard debugging output
#DEFINE "[MAP]","'"  'enable MAP debugging
#DEFINE "[DRN]","'"  'enable Drohne debugging
#DEFINE "[AI]",""   'enable AI debugging

CONST PWD$=getParent$(MM.Info(current))

#Include "inc/game.inc"
#Include "inc/controls.inc"  'joystick controls
#Include "inc/settings.inc"
#Include "inc/cmap.inc"      'colormap functions
#Include "inc/map.inc"
#Include "inc/level.inc"
#Include "inc/player.inc"
#Include "inc/panzer.inc"
#Include "inc/panzerAI.inc"
#Include "inc/drohne.inc"
#Include "inc/shots.inc"
#Include "inc/explosion.inc"
#Include "inc/particle.inc"
#Include "inc/utils.inc"
#Include "inc/sound.inc"

#Include "inc/page_intro.inc"
#Include "inc/page_config.inc"

game.init()
game.loadAssets()

DIM Integer Screen.W=MM.HRES
DIM Integer Screen.H=MM.VRES
DIM Integer Screen.VPx=0
DIM Integer Screen.VPy=80

'Area of Playfield on the screen
DIM Integer VP.W=Screen.W-Screen.VPx
DIM Integer VP.H=Screen.H-Screen.VPy

'Area where tanks can move, relative to VP
DIM Integer Playfield.X=Screen.VPx+18
DIM Integer Playfield.Y=Screen.VPy+26
DIM Integer Playfield.W=604
DIM Integer Playfield.H=348

page display PAGE_DISPLAY
page write PAGE_BUFFER

'**********************************************************
'*                     Game Main Loop                     *
'**********************************************************
DIM Integer screenshot
DIM Integer key,one,player,rc,panzer
DIM Integer Inpch,ctrl
DIM Float   x,y
DIM Float   tim

do
  Game.clrScreen()

'********************> State Handling <********************
  select case Game.State
  case STATE_INTRO
    if one=0 then one=1 : Intro.init
    if isESC() then exit do
  
    Intro.draw
    Game.NumPlayers=Intro.update()
    if Game.NumPlayers>0 then
      cls
      changeState(STATE_CONFIG)
      Player.reset
      Config.init Game.NumPlayers
      one=1
    endif

  case STATE_CONFIG
    if one=0 then one=1 : Config.reset
    if isESC() then changeState(STATE_INTRO)

    Config.draw
    if Config.update()=1 then
      Level.load(Game.NumPlayers)
      if Game.NumPlayers=1 then Player.setRobot(2)
      changeState(STATE_GAME)
    endif

  case STATE_GAME
    if one=0 then one=1 : Game.start : Player.newgame() : Panzer.Sound=1
    if isESC() then cls : changeState(STATE_CONFIG)

    for player=0 to Game.NumPlayers-1
      inpch=Player.getInpch(player+1)
      ctrl=Controls.read(inpch)

      if Panzer.isActive(player) then      
        if (ctrl and 1) > 0 then 'left
          if (ctrl and 12) > 0 then
            Panzer.turn(player,-1)
          else
            Panzer.cannon(player,-1)
          endif
        endif

        if (ctrl and 2) > 0 then 'right
          if (ctrl and 12) > 0 then
            Panzer.turn(player,1)
          else
            Panzer.cannon(player,1)
          endif
        endif

        if (ctrl and 4) > 0 then 'up
          Panzer.move(player,0.5)
        endif

        if (ctrl and 8) > 0 then 'down
          Panzer.move(player,-0.5)
        endif

        if (ctrl and 16) > 0 then 'fire
          if Game.Ready=0 then Panzer.fire(player)
        endif
      endif

      if (ctrl and 32) > 0 then 'repair
        if Player.getLives(player+1)>0 then
          if Panzer.repair(player) then Player.decLives(player+1)
        endif
      endif
    next

    if Player.active()=1 then changeState(STATE_VICTORY)

    Game.update
    Particle.draw
    Game.draw

    AI.update(Panzer.X(0),Panzer.Y(0))


  case STATE_VICTORY
    if one=0 then one=1 : player=Player.getWinner()  ' : playSample 8,5512,1
    if isESC() then changeState(STATE_CONFIG)

    Game.update
    Game.draw

    x=Screen.VPx+VP.W/2 : y=Screen.VPy+VP.H/2-30
    if TIMER-tim>5000 then
      tim=TIMER-RND()*2000
      select case int(RND()*3)
      case 0
        Emitter.rocket1 x,y,RAD(150+RND()*60),120+RND()*30,"Emitter.fw1"
      case 1  
        Emitter.rocket2 x,y,RAD(150+RND()*60),120+RND()*30,"Emitter.fw2"
      case 2
        Emitter.rocket2 x,y,RAD(150+RND()*60),120+RND()*30,"Emitter.fw3"
      end select
    endif
    Particle.draw

    x=Screen.VPx+(VP.W-239)/2 : y=Screen.VPy+(VP.H-189)/2
    blit 0,96,x,y,234,189,PAGE_SPRITES,&B100
    text Screen.W/2,y+88,"Player","C",3,,map(130),-1
    text Screen.W/2,y+116,"#"+str$(player),"C",3,,map(130),-1

    key=controls.readKey()
    if key=32 then changeState(STATE_CONFIG)
  end select
  
  Game.swapPage()
loop

'***********************> Game Exit Handling <***********************
settings.save()
mode 1
page write 0
print "Good Bye..."

sub changeState(newstate%)
  Game.State=newstate%
  playSample 14,22050,1
  Panzer.Sound=0
  one=0
end sub

function isESC() as Integer
  STATIC Integer screenshot=0,oldKey=0
  LOCAL Integer key

  key=Controls.readKey()
  if oldKey<>Key then
    oldKey=Key
    if key=27 then isESC=1 : exit function
    if key=147 then save image "screenshot"+str$(screenshot)+".bmp":inc screenshot 'F3 for screenshot
  endif
end function

'function getWinner() as Integer
'  LOCAL Integer player

'  for player=1 to Game.NumPlayers
'    if Player.isActive(player) then getWinner=player
'  next
'end function


