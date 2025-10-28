
CONST INTRO_SPEED=20
CONST ISTATE_CREDITS=0
CONST ISTATE_PLAYER=1

DIM Integer Intro.state
DIM Integer Intro.max_players
DIM Integer Intro.num_players
DIM Float   Intro.tiAnim

DIM Float Title.X
DIM Float Title.Y
DIM Float Title.phi
DIM Float Title.scale=150
DIM Float Title.steps=1200

DIM Float Logo.X
DIM Float Logo.Y
DIM Float Logo.phi
DIM Float Logo.scale=100
DIM Float Logo.steps=1500

sub Intro.init()
  LOCAL Integer ctrls=Controls.getNumControls()

  Intro.max_players=choice(ctrls>4,4,ctrls)
  Intro.num_players=1
  Intro.state=ISTATE_CREDITS

  Title.phi=PI/2  'Initial condition for anmim
  Logo.phi=0

  mode 8,16
  page write PAGE_BUFFER
  playMOD "music"
end sub

sub Intro.draw()
  LOCAL Float cosphi,sinphi
  LOCAL Float lf,cx,cy

  if TIMER-Intro.tiAnim>INTRO_SPEED then
    Intro.tiAnim=TIMER

    cosphi=cos(Title.phi) : sinphi=sin(Title.phi)
    Title.X=(Screen.W-281)/2+Title.scale*cosphi/(1+sinphi*sinphi)
    Title.Y=(3*Screen.H/8-53/2)+Title.scale*cosphi*sinphi/(1+sinphi*sinphi)
    inc Title.phi,PI/Title.steps : if Title.phi>2*PI then inc Title.phi,-2*PI

    Logo.X=(Screen.W-360)/2+Logo.scale*sin(Logo.phi)
    inc Logo.phi,PI/Logo.steps : if Logo.phi>2*PI then inc Logo.phi,-2*PI
  endif

  lf=18 : cx=Screen.W/2 : cy=250
  select case Intro.state
  case ISTATE_CREDITS
    text cx,cy,"Credits","C",4,,rgb(168,102,26)
    line cx-155,cy+15,cx+155,cy+15,1,rgb(168,102,26) : inc cy,lf
    text cx-10,cy,"Programming:","R",4,,rgb(89,85,73)
    text cx,cy,"Matthias Grimm","L",4,,rgb(188,186,180) : inc cy,lf
    text cx-10,cy,"Graphics:","R",4,,rgb(89,85,73)
    text cx,cy,"Matthias Grimm","L",4,,rgb(188,186,180) : inc cy,lf
    text cx-10,cy,"Sound Fx:","R",4,,rgb(89,85,73)
    text cx,cy,"Matthias Grimm","L",4,,rgb(188,186,180) : inc cy,lf
    text cx,cy,"Dominik Braun","L",4,,rgb(188,186,180) : inc cy,lf
    text cx-10,cy,"Music:","R",4,,rgb(89,85,73)
    text cx,cy,"Songerson","L",4,,rgb(188,186,180)
    line cx-155,cy+15,MM.HRES/2+155,cy+15,1,rgb(168,102,26) : inc cy,lf
    text cx,cy,"Parts of the game Gfx was created with GenAI","C",7,,rgb(109,70,27) : inc cy,10
    text cx,cy,"(c) 2025","C",7,,rgb(109,70,27) : inc cy,15
    text cx,cy,"Press SPACE to start the Game","C",7,,rgb(green)

  case ISTATE_PLAYER
    text cx,cy,"Select # of Players","C",4,,rgb(168,102,26)
    line cx-165,cy+15,cx+165,cy+15,1,rgb(168,102,26) : inc cy,lf+15
    text cx,cy,str$(Intro.num_players),"C",6,,rgb(89,85,73) : inc cy,50
    line cx-165,cy+15,cx+165,cy+15,1,rgb(168,102,26) : inc cy,lf
    text cx,cy,"Nunckuk or cursor keys, ENTER to select","C",7,,rgb(109,70,27)
  end select

  blit 0,0,Logo.X,Logo.Y,360,240,PAGE_TITLE,&B100
  blit 0,240,Title.X,Title.Y,281,53,PAGE_TITLE,&B100
end sub

'returns number of selected players or 0 if the selection has not been made yet
function Intro.update() as Integer
  STATIC Integer lock
  LOCAL Integer key,num
  
  Intro.update=0
  if keydown(0)>0 then
    key=keydown(1) : if key=lock then exit function
    lock=key

    select case Intro.state
    case ISTATE_CREDITS
      if key=32 then Intro.state=ISTATE_PLAYER : playMOD "keysound"
    case ISTATE_PLAYER
      num=Intro.num_players
      if key=32 then num=choice(num<Intro.max_players,num+1,1)
      if key=129 then num=choice(num<Intro.max_players,num+1,num)
      if key=128 then num=choice(num>1,num-1,num)
      if key>48 and key<60 then num=choice(key-48<=Intro.max_players,key-48,num)
      if Intro.num_players<>num then playSample 1,22050
      Intro.num_players=num
    
      if key=13 or key=10 then
        playSample 2,30000
        pause 100 'necessary to play sound fx
        Intro.update=Intro.num_players
      endif
    end select
  else
    lock=0
  endif
end function






