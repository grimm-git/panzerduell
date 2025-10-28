'Game page Configuration with input selection, configuration

CONST CONFIG_MAXPLAYER=4
CONST CONFIG_BORDER=2*10
CONST CONFIG_TOP=230

CONST CSTATE_DEBUG =&HFFE
CONST CSTATE_TRANS =&HFFF
CONST CSTATE_NONE  =&H000
CONST CSTATE_INPUT =&H100
CONST CSTATE_MENU  =&H200
CONST CSTATE_MSKEY =&H210
CONST CSTATE_MRKEY =&H211
CONST CSTATE_MCAL  =&H220
CONST CSTATE_MCJOY =&H221
CONST CSTATE_MCJOYE=&H222
CONST CSTATE_MCACC =&H223
CONST CSTATE_MCACCE=&H224
CONST CSTATE_START =&H300

DIM Integer Config.numPlayers
DIM Integer Config.State(CONFIG_MAXPLAYER-1)
DIM Integer Config.trans(CONFIG_MAXPLAYER-1)
DIM Integer Config.transtime(CONFIG_MAXPLAYER-1)
DIM Integer Config.cursor(CONFIG_MAXPLAYER-1)
DIM Integer Config.keylock

sub Config.init(players%)
  LOCAL Integer n

  Config.numPlayers=players%
  for n=0 to CONFIG_MAXPLAYER-1
    Config.State(n)=CSTATE_NONE
    Config.trans(n)=CSTATE_NONE
    Config.transtime(n)=0
    Config.cursor(n)=0
    Config.keylock=0
  next

  Config.State(0)=CSTATE_INPUT  'Start input phase for player 1

  mode 8,16
end sub

sub Config.reset
  LOCAL Integer n

  for n=0 to CONFIG_MAXPLAYER-1
    Config.State(n)=CSTATE_MENU
    Config.trans(n)=CSTATE_NONE
    Config.transtime(n)=0
    Config.cursor(n)=0
    Config.keylock=0
  next

  mode 8,16
  page write PAGE_BUFFER
end sub

sub Config.draw()
  LOCAL Integer inpch,dev,idx
  LOCAL Float share,nl,cx,cy
  LOCAL String txt$

  blit 0,0,(Screen.W-360)/2,0,360,240,PAGE_TITLE,&B100
  blit 0,240,(Screen.W-281)/2,3*Screen.H/8-53/2,281,53,PAGE_TITLE,&B100

  share=(MM.HRES-CONFIG_BORDER)/Config.numPlayers  
  nl   = MM.INFO(FONTHEIGHT)

  for idx=0 to Config.numPlayers-1
    cx=idx*share+share/2 : cy = CONFIG_TOP
    text cx,cy,"Player "+str$(idx+1),"C",,1,Player.getColor(idx+1)
  
    inc cy,12
    inpch=Player.getInpch(idx+1)
    if inpch>=0 then
      dev=Controls.getType(inpch)
      blit dev*128,0,cx-64,cy,128,128,PAGE_DEVICES,&B100
      inc cy,128
    endif

    select case Config.State(idx)
    case CSTATE_INPUT
      txt$=chr$(22)+Controls.getFireButton(0)+chr$(22)+", "+chr$(22)+Controls.getFireButton(1)+chr$(22)+"or Num-Z"

      inc cy,12
      text cx,cy,"Please press either","C",,1,Player.getColor(idx+1)
      text cx,cy+nl,txt$,"C",,1,Player.getColor(idx+1)

    case CSTATE_MENU 'requires inpch=0..3
      text cx,cy,"Start Game","C",,1,rgb(white)

      select case dev
      case CTRL_TYPE_KEYBOARD
        text cx,cy+nl,"Show Keys","C",,1,rgb(white)
        text cx,cy+nl*2,"Reassign Keys","C",,1,choice(Config.keylock,rgb(grey),rgb(white))
      case CTRL_TYPE_NUNCHUK
        text cx,cy+nl,"Calibrate","C",,1,rgb(grey)
[NUN]   text cx,cy+nl,"Calibrate","C",,1,rgb(white)
      case CTRL_TYPE_JOYSTICK
      end select

      cy=cy+Config.cursor(idx)*nl-1
      rbox cx-64,cy,128,nl+1,4,Player.getColor(idx+1)
    
    case CSTATE_MSKEY,CSTATE_MRKEY
      text cx,cy+2*nl,"Left:    '"+chr$(Controls.keys(inpch,0))+"'","C",,1,rgb(white)
      text cx,cy+3*nl,"Right:   '"+chr$(Controls.keys(inpch,1))+"'","C",,1,rgb(white)
      text cx,cy+4*nl,"Up:      '"+chr$(Controls.keys(inpch,2))+"'","C",,1,rgb(white)
      text cx,cy+5*nl,"Down:    '"+chr$(Controls.keys(inpch,3))+"'","C",,1,rgb(white)
      text cx,cy+6*nl,"Fire:    '"+chr$(Controls.keys(inpch,4))+"'","C",,1,rgb(white)
      text cx,cy+7*nl,"Repair:  '"+chr$(Controls.keys(inpch,5))+"'","C",,1,rgb(white)

      if Config.State(idx)=CSTATE_MSKEY then
        text cx,cy,"Key Assignments","C",,1,Player.getColor(idx+1)
      else
        text cx,cy,"Reassign Keys","C",,1,Player.getColor(idx+1)
        text cx,cy+nl,"Press new Key for","C",,1,rgb(white)

        cy=cy+nl*2+Config.Cursor(idx)*nl-1
        rbox cx-64,cy,128,nl+1,4,Player.getColor(idx+1)
      endif

    case CSTATE_MCAL
      text cx,cy,"Calibration","C",,1,rgb(white)
      text cx,cy+2*nl,"center the joystick","C",,1,rgb(white)
      text cx,cy+3*nl,"and press C-Button","C",,1,rgb(white)

    case CSTATE_MCJOY,CSTATE_MCJOYE
      text cx,cy,"Calibration #1","C",,1,rgb(white)
      text cx,cy+2*nl,"Move joystick in circles","C",,1,rgb(white)
      text cx,cy+3*nl,"at full scale positions","C",,1,rgb(white)
      _drawBusyJoy(cx,cy+6*nl,Nunchuk.cal_mask(Controls.no(inpch)))
      if Config.State(idx)=CSTATE_MCJOYE then
        text cx,cy+8*nl,"Calibration complete","C",,1,rgb(white)
      endif
    case CSTATE_MCACC,CSTATE_MCACCE
      text cx,cy,"Calibration #2","C",,1,rgb(white)
      text cx,cy+2*nl,"Rotate the nunchuk slowly","C",,1,rgb(white)
      text cx,cy+3*nl,"a full turn in two axes","C",,1,rgb(white)
      _drawBusyACC(cx,cy+6*nl,Nunchuk.cal_mask(Controls.no(inpch)))
      if Config.State(idx)=CSTATE_MCACCE then
        text cx,cy+8*nl,"Calibration complete","C",,1,rgb(white)
      endif
    case CSTATE_DEBUG
      text cx,cy,"Calibration Report","C",,1,rgb(white)
    
      dev=Controls.getNunchuk(inpch)
      txt$=str$(Nunchuk.cal.jox(dev,NUNCHUK_LOW))
      txt$=txt$+" < "+str$(Nunchuk.cal.jox(dev,NUNCHUK_CENTER))
      txt$=txt$+" < "+str$(Nunchuk.cal.jox(dev,NUNCHUK_HIGH))
      text cx,cy+2*nl,"Joy X: "+txt$,"C",,1,rgb(white)

      txt$=str$(Nunchuk.cal.joy(dev,NUNCHUK_LOW))
      txt$=txt$+" < "+str$(Nunchuk.cal.joy(dev,NUNCHUK_CENTER))
      txt$=txt$+" < "+str$(Nunchuk.cal.joy(dev,NUNCHUK_HIGH))
      text cx,cy+3*nl,"Joy Y: "+txt$,"C",,1,rgb(white)

      text cx,cy+5*nl,"Acc X: "+str$(Nunchuk.cal.ag1(dev,NUNCHUK_X)),"C",,1,rgb(white)
      text cx,cy+6*nl,"Acc Y: "+str$(Nunchuk.cal.ag1(dev,NUNCHUK_Y)),"C",,1,rgb(white)
      text cx,cy+7*nl,"Acc Z: "+str$(Nunchuk.cal.ag1(dev,NUNCHUK_Z)),"C",,1,rgb(white)

    case CSTATE_START
      text cx,cy+2*nl,"Wait for","C",,1,rgb(white)
      text cx,cy+3*nl,"other players","C",,1,rgb(white)

    end select
  next
end sub

function Config.update() as Integer
  LOCAL Integer inpch,ctrl,dev,cursor
  LOCAL Integer mask,done,idx,newstate

  for idx=0 to Config.numPLayers-1
    select case Config.State(idx)
    case CSTATE_TRANS
      if _isControlBusy(idx)<>0 then exit function
      Config.State(idx)=Config.Trans(idx)
      Config.Trans(idx)=CSTATE_NONE

    case CSTATE_INPUT
      inpch=Controls.learn(idx+1)
      if inpch>=0 then
        if Player.setInpch(idx+1,inpch)=1 then
          _initTransition(idx,CSTATE_MENU)
          if idx+1<Config.numPLayers then Config.State(idx+1)=CSTATE_INPUT
          playSample 2,30000
        endif
      endif

    case CSTATE_MENU
      newstate=_handleMainMenu(idx)
      if newstate>0 then _initTransition(idx,newstate) : Config.cursor(idx)=0

    case CSTATE_MSKEY
      inpch=Player.getInpch(idx+1)
      ctrl=Controls.read(inpch)
      if ctrl<>0 then
        playSample 2,30000
        _initTransition(idx,CSTATE_MENU)
      endif

    case CSTATE_MRKEY
      Config.keylock=1
      key=Controls.inKey()
      if key>0 then
        inpch=Player.getInpch(idx+1)

        if not Controls.isKeyUsed(inpch,key) then
          Controls.keys(inpch,Config.cursor(idx))=key
          inc Config.cursor(idx)
          if Config.cursor(idx)>5 then
            Config.cursor(idx)=0
            Config.keylock=0
            _initTransition(idx,CSTATE_MENU)
          endif
        endif
      endif

    case CSTATE_MCAL
      inpch=Player.getInpch(idx+1)
      
      ctrl=Controls.read(inpch)
      if (ctrl and 32) > 0 then _initTransition(idx,CSTATE_MCJOY)  'aux key (C)

    case CSTATE_MCJOY
      inpch=Player.getInpch(idx+1)
      dev=Controls.getNunchuk(inpch)

      mask=Nunchuk.calibrateJoy(dev)
      if mask=&B111111 then
        Config.trans(idx%)=CSTATE_MCACC
        Config.state(idx%)=CSTATE_MCJOYE
      elseif mask=0 then
        _initTransition(idx,CSTATE_MCAL)
      endif

    case CSTATE_MCACC
      inpch=Player.getInpch(idx+1)
      dev=Controls.getNunchuk(inpch)

      mask=Nunchuk.calibrateACC(dev)
      if mask=&B111 then
        Config.trans(idx%)=CSTATE_MENU
        Config.state(idx%)=CSTATE_MCACCE
      endif

    case CSTATE_MCJOYE,CSTATE_MCACCE
      if Config.transtime(idx%)=0 then Config.transtime(idx%)=TIMER
      if TIMER-Config.transtime(idx%)>2000 then
        Config.transtime(idx%)=0
        Config.state(idx%)=CSTATE_TRANS
      endif    

    case CSTATE_DEBUG
      if keydown(0)>0 then
        if keydown(1)=32 then _initTransition(idx,CSTATE_MENU)
      endif

    case CSTATE_START
      done=done or 2^idx
    end select
  next
  Config.update=choice(done=2^Config.numPLayers-1,1,0)
end function

sub _drawBusyJoy(cx!,cy!,mask%)
  circle cx!,cy!,2,0,1,0,choice(mask% and &B010010,rgb(green),rgb(red))
  circle cx!-13,cy!,2,0,1,0,choice(mask% and &B100000,rgb(green),rgb(red))
  circle cx!+13,cy!,2,0,1,0,choice(mask% and &B001000,rgb(green),rgb(red))
  circle cx!,cy!-13,2,0,1,0,choice(mask% and &B000100,rgb(green),rgb(red))
  circle cx!,cy!+13,2,0,1,0,choice(mask% and &B000001,rgb(green),rgb(red))
  line cx!-6,cy!,cx!-9,cy!,1,rgb(white)
  line cx!+6,cy!,cx!+9,cy!,1,rgb(white)
  line cx!,cy!-6,cx!,cy!-9,1,rgb(white)
  line cx!,cy!+6,cx!,cy!+9,1,rgb(white)
end sub

sub _drawBusyACC(cx!,cy!,mask%)
  LOCAL Integer d

  d=choice(mask% and &B100,48,0)
  blit 512+d, 0,cx!-80,cy!-21,48,43,PAGE_DEVICES,&B100   'x
  d=choice(mask% and &B010,48,0)
  blit 512+d,43,cx!-24,cy!-21,48,43,PAGE_DEVICES,&B100   'y
  d=choice(mask% and &B001,48,0)
  blit 512+d,86,cx!+32,cy!-21,48,43,PAGE_DEVICES,&B100   'z
end sub

function _isControlBusy(idx%) as Integer
  LOCAL Integer inpch

  inpch=Player.getInpch(idx%+1)
  _isControlBusy=Controls.read(inpch)
end function

'returns the CSTATE of the selected item or 0
function _handleMainMenu(idx%) as Integer
  LOCAL Integer ctrl,inpch,dev
  LOCAL Integer cursor,curmax

  cursor=Config.cursor(idx%)
  inpch=Player.getInpch(idx%+1)
  dev=Controls.getType(inpch)

  'menu entries for KEYBOARD, NUNCHUK and JOYSTICK, start at 0
[NUN]  curmax=choice(dev=CTRL_TYPE_NUNCHUK,1,0)
  curmax=choice(dev=CTRL_TYPE_KEYBOARD,choice(Config.keylock,1,2),curmax)

  ctrl=Controls.read(inpch)
  if (ctrl and 4) > 0 then  'up
    if Controls.getTime(inpch,CTRL_UP)<=1 then
      inc cursor,-1
      if cursor<0 then cursor=curmax
      playSample 1,22050
     endif
  elseif (ctrl and 8) > 0 then 'down
    if Controls.getTime(inpch,CTRL_DOWN)<=1 then
      inc cursor
      if cursor>curmax then cursor=0
      playSample 1,22050
    endif
  elseif (ctrl and 16) > 0 then 'select
    if Controls.getTime(inpch,CTRL_FIRE)<=1 then
      if cursor=0 then
        _handleMainMenu=CSTATE_START
      elseif cursor=1 then
        _handleMainMenu=choice(dev=CTRL_TYPE_KEYBOARD,CSTATE_MSKEY,CSTATE_MCAL)
      elseif cursor=2 then
        _handleMainMenu=CSTATE_MRKEY
      endif
      playSample 2,30000
    endif
  endif

  Config.cursor(idx%)=cursor
end function

sub _initTransition(idx%,newstate%)
  Config.trans(idx%)=newstate%
  Config.state(idx%)=CSTATE_TRANS
end sub


