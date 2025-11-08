' Include controls.inc
' Contains code to read player's input

' Support for max four players (1..4)
' Nunchuck, keyboard or joystick configurable (joystick currently not implemented)

CONST CTRL_MAXINPUTS=4  '2x keyboard, 2x Nunchuk

CONST CTRL_RIGHT=0
CONST CTRL_LEFT =1
CONST CTRL_UP   =2
CONST CTRL_DOWN =3
CONST CTRL_FIRE =4
CONST CTRL_AUX  =5

CONST CTRL_TYPE_NONE=0
CONST CTRL_TYPE_KEYBOARD=1
CONST CTRL_TYPE_NUNCHUK=2
CONST CTRL_TYPE_JOYSTICK=3

DIM Integer Controls.type(CTRL_MAXINPUTS)    'One of CTRL_TYPE_xxx
DIM String  Controls.id(CTRL_MAXINPUTS)      'Name of Controller
DIM Float   Controls.ticks(CTRL_MAXINPUTS,5)
DIM Integer Controls.keys(CTRL_MAXINPUTS,5)  'Keybrd:  Keys for Joystick function
DIM Integer Controls.no(CTRL_MAXINPUTS)      'Nunchuk: I2C channel

sub Controls.init()
  LOCAL Integer n,inpch,nun
  LOCAL String name$,ppos$

  for inpch=0 to 3
    select case inpch
    case 0
      Controls.type(inpch)=CTRL_TYPE_KEYBOARD
      Controls.id(inpch)="Keyboard left"
      Controls.no(inpch)=0
      for n=0 to 4
        Controls.ticks(inpch,n)=0
      next
      Controls.keys(inpch,CTRL_RIGHT)= asc("a")
      Controls.keys(inpch,CTRL_LEFT) = asc("d")
      Controls.keys(inpch,CTRL_UP)   = asc("w")
      Controls.keys(inpch,CTRL_DOWN) = asc("s")
      Controls.keys(inpch,CTRL_FIRE) = asc("q")
      Controls.keys(inpch,CTRL_AUX)  = asc("e")
    case 1
      Controls.type(inpch)=CTRL_TYPE_KEYBOARD
      Controls.id(inpch)="Keyboard right"
      Controls.no(inpch)=0
      for n=0 to 4
        Controls.ticks(inpch,n)=0
      next
      Controls.keys(inpch,CTRL_RIGHT)= asc("=")
      Controls.keys(inpch,CTRL_LEFT) = asc(":")
      Controls.keys(inpch,CTRL_UP)   = asc("@")
      Controls.keys(inpch,CTRL_DOWN) = asc(";")
      Controls.keys(inpch,CTRL_FIRE) = asc("p")
      Controls.keys(inpch,CTRL_AUX)  = asc("*")
    case 2,3
      nun=choice(inpch=2,3,1)
      ON ERROR SKIP
      Controller Nunchuk open nun
      if MM.ERRNO=0 then
        ppos$=choice(inpch=2,"Inner","Outer")
        name$="Nunchuk, "+ppos$+" Port"

        Controls.type(inpch)=CTRL_TYPE_NUNCHUK
        Controls.id(inpch)=name$
        Controls.no(inpch)=nun
        for n=0 to 4
          Controls.ticks(inpch,n)=0
        next
      else
        Controls.type(inpch)=CTRL_TYPE_NONE
      endif
    end select
  next inpch
end sub

function Controls.getNumControls() as Integer
  LOCAL Integer cnt,n

  for n=0 to 4
    if Controls.type(n)<>CTRL_TYPE_NONE then inc cnt
  next
  Controls.getNumControls=cnt
end function

function Controls.getFireButton(inpch%) as String
  select case Controls.type(inpch%)
  case CTRL_TYPE_KEYBOARD
    Controls.getFireButton=chr$(Controls.keys(inpch%,CTRL_FIRE))
  case CTRL_TYPE_NUNCHUK
    Controls.getFireButton="Z"
  case CTRL_TYPE_JOYSTICK
    Controls.getFireButton="Fire"
  end select
end function

function Controls.getType(inpch%) as Integer
  Controls.getType=Controls.type(inpch%)
end function

function Controls.isKeyUsed(inpch%, key%) as Integer
  LOCAL Integer n,rc,ch=(inpch%+1) and 1

  for n=0 to 5
    if key%=Controls.keys(ch,n) then rc=1 : exit for
  next
  Controls.isKeyUsed=rc
end function


'returns 0..4 = learned input chanel for a player
'          -1 = input channel has not been learned yet
'          -2 = error
function Controls.learn(player%) as Integer
  LOCAL Integer key=0
  LOCAL Integer n,m,found=-1,nun

  for n=0 to CTRL_MAXINPUTS
    select case Controls.type(n)
      case CTRL_TYPE_KEYBOARD
        key=keydown(1)
        if key<>0 then
          for m=0 to 5
            if key=Controls.keys(n,m) then
              Controls.learn=n
              exit function
            endif
          next m
        endif
      case CTRL_TYPE_NUNCHUK
        nun=Controls.no(n)
        if Nunchuk(Z,nun)+Nunchuk(C,nun)>0 then
          Controls.learn=n
          exit function
        endif
      case else
        Controls.learn=-2
    end select
  next n
  Controls.learn=-1
end function

'return: 1=inpch is a Nunchuk
function Controls.isNunchuk(inpch%) as Integer
  Controls.isNunchuk=Controlstype(inpch%)=CTRL_TYPE_NUNCHUK
end function

'return: nunchuk's I2C channel or 0 if the control is not a Nunchuk
function Controls.getNunchuk(inpch%) as Integer
  Controls.getNunchuk=Controls.no(inpch%)
end function

function Controls.getID(inpch%) as String
  Controls.getID=Controls.id(inpch%)
end function

function Controls.getTime(inpch%,idx%) As Float
  Controls.getTime=Controls.ticks(inpch%,idx%)
end function

' Returns player's input as bitcoded integer
' %0000 0000 - no input
' %0000 0001 - right
' %0000 0010 - left
' %0000 0100 - up
' %0000 1000 - down
' %0001 0000 - fire button
' %0010 0000 - auxillary button
function Controls.read(inpch%) as Integer
  LOCAL Integer x,y,b,n,m
  LOCAL Integer ctrl=0
  LOCAL Integer keyCount=0
  LOCAL Integer nun

  select case Controls.type(inpch%)
    case CTRL_TYPE_NUNCHUK
      nun=Controls.no(inpch%)
      x=Nunchuk(JX,nun)
      if x<64 then ctrl=ctrl or 1
      if x>192 then ctrl=ctrl or 2
      y=Nunchuk(JY,nun)
      if y<64 then ctrl=ctrl or 8
      if y>192 then ctrl=ctrl or 4
      b=Nunchuk(Z,nun)
      if b=1 then ctrl=ctrl or 16 ' fire button
      b=Nunchuk(C,nun)
      if b=1 then ctrl=ctrl or 32 ' auxillary button

    case CTRL_TYPE_KEYBOARD
      keyCount=keydown(0)
      for n=1 to keyCount
        for m=0 to 5
          if keydown(n)=Controls.keys(inpch%,m) then ctrl=ctrl or 2^m
        next m
      next n
  end select

  _countCtrlTicks(inpch%,ctrl)
  Controls.read%=ctrl
end function

function Controls.readKey() as Integer
  LOCAL Integer key=0
  LOCAL Integer n,nun

  for n=0 to CTRL_MAXINPUTS
    select case Controls.type(n)
      case CTRL_TYPE_KEYBOARD
        if keydown(0)<>0 then key=keydown(1)
      case CTRL_TYPE_NUNCHUK
        nun=Controls.no(n)
        if Nunchuk(Z,nun)+Nunchuk(C,nun)>0 then key=32
    end select
    if key<>0 then exit for
  next n
  Controls.readKey=key
end function

function Controls.inKey() as Integer
  STATIC Integer oldkey=0
  LOCAL Integer key

  if keydown(0)>0 then
    key=keydown(1)
    if key>0 and key<>oldkey then oldkey=key : Controls.inkey=key
  else
    oldkey=0
  endif
end function


'Private functions
sub _countCtrlTicks(inpch%, ctrl%)
  LOCAL Integer n

  for n=0 to 5
    if (ctrl% and (2^n)) then
      inc Controls.ticks(inpch%,n)
    else
      Controls.ticks(inpch%,n)=0
    endif
  next
end sub

