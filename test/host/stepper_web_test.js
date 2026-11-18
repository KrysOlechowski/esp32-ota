// Test zachowania przycisków z symulowanym API; bez urządzenia i pomiarów fizycznych.
const fs = require('node:fs');
const vm = require('node:vm');
const assert = require('node:assert/strict');
const source = fs.readFileSync('include/web_page.h', 'utf8');
const script = source.match(/<script>([\s\S]*?)<\/script>/)[1];
const elements = new Map();
for (const [, id] of source.matchAll(/id="([^"]+)"/g)) {
  elements.set(id, {value:'',textContent:'',disabled:false,checkValidity(){return true},reportValidity(){},style:{},classList:{toggle(){},add(){},remove(){}}});
}
elements.get('servoAngle').value = '90';
elements.get('servoSpeed').value = '50';
elements.get('motorDegrees').value = '90';
elements.get('motorSpeed').value = '100';
elements.get('lightEffectSelect').value='1';elements.get('motorAnim').value='swing';elements.get('servoAnim').value='wave';
elements.get('color').value = '#0064ff';
elements.get('brightness').value = '100';
elements.get('saturation').value = '100';
let state = {device:'test',temperature:20,humidity:50,sensorOK:true,pwmOK:true,enabled:false,
  mode:'manual',ageSeconds:0,color:'#000000',manualColor:'#0064ff',brightness:100,secondMode:'sync',secondManualBrightness:100,
  manualSaturation:100,cold:17,warm:22,dry:30,wet:70,motorRunning:false,motorDirection:0,servoActive:false,servoOK:true,servoTarget:90};
let calls = [], fail = false;
const context = vm.createContext({URLSearchParams,document:{getElementById:id=>elements.get(id),
  querySelectorAll:selector=>[...elements].filter(([id])=>selector.includes(':not')&&!id.startsWith('motor')).map(([,el])=>el)},
  setTimeout(){},fetch:async(path,options)=>{
    if(options?.body){
      const data=Object.fromEntries(options.body);
      calls.push({path,data});
      if(fail)throw Error('offline');
      if(path==='/api/light-effect')state={...state,lightEffect:Number(data.effect)};
      if(path==='/api/light-test'){
        const colors=['#ff0000','#00ff00','#0000ff','#ff00ff','#ff0064'];
        const color=colors[Number(data.color)],duty=color.match(/[a-f0-9]{2}/gi).map(v=>Math.round((parseInt(v,16)/255)**2.2*255));
        state={...state,lightEffect:0,enabled:true,mode:'manual',manualColor:color,color,manualSaturation:100,brightness:100,
          secondMode:'sync',secondColor:color,secondBrightness:100,secondGainR:100,secondGainG:100,secondGainB:100,firstDuty:duty,secondDuty:duty};
      }
      if(path==='/api/motion-effect')state={...state,motionEffect:data.effect==='stop'?0:data.device==='motor'?1:4,motionLoop:data.loop==='1'};
      if(path==='/api/second-balance')state={...state,secondGainR:Number(data.r),secondGainG:Number(data.g),secondGainB:Number(data.b)};
      if(path==='/api/second')state={...state,secondMode:data.mode,secondColor:data.mode==='manual'?data.color:'#000000',secondBrightness:Number(data.brightness??0)};
      if(path==='/api/servo')state={...state,servoActive:['move','home'].includes(data.action),servoHasCommand:true,servoZeroKnown:state.servoZeroKnown||data.action==='zero',servoZero:data.action==='zero'?state.servoTarget:state.servoZero,servoTarget:Number(data.angle??state.servoTarget)};
      if(path==='/api/motor')state={...state,motorRunning:!['stop','zero','home'].includes(data.action),motorDirection:data.action==='stop'?0:data.action==='left'?-1:1,motorDiagnostic:data.action==='test'};
    }
    return {ok:true,json:async()=>({...state})};
  }});
(async()=>{
  vm.runInContext(script,context);
  await new Promise(resolve=>setImmediate(resolve));
  assert.equal(elements.get('brightness').value,100);
  assert.equal(elements.get('secondBrightness').value,100);
  await elements.get('motorLeft').onclick();
  assert.deepEqual(calls.at(-1),{path:'/api/motor',data:{action:'left',degrees:'90',speed:'100'}});
  assert.equal(elements.get('servoMove').disabled,true);
  assert.equal(elements.get('motorRight').disabled,true);
  assert.equal(elements.get('motorStop').disabled,false);
  const count=calls.length;
  await elements.get('motorRight').onclick();
  assert.equal(calls.length,count); // aktywny ruch nie jest przedłużany
  vm.runInContext('busy=true',context); // Stop także podczas żądania lampki
  await elements.get('motorStop').onclick();
  assert.deepEqual(calls.at(-1).data,{action:'stop'});
  vm.runInContext('busy=false',context);
  assert.equal(elements.get('motorLeft').disabled,false);
  assert.match(elements.get('motorState').textContent,/cewki wyłączone/);
  await elements.get('motorTest').onclick();
  assert.deepEqual(calls.at(-1).data,{action:'test'});
  assert.match(elements.get('motorState').textContent,/Test A–D/);
  assert.equal(elements.get('motorTest').disabled,true);
  await elements.get('motorStop').onclick();
  assert.equal(elements.get('motorTest').disabled,false);
  await elements.get('motorFullRight').onclick();
  assert.deepEqual(calls.at(-1).data,{action:'right',degrees:'360',speed:'100'});
  await elements.get('motorStop').onclick();
  await elements.get('motorZero').onclick();
  assert.deepEqual(calls.at(-1).data,{action:'zero'});
  fail=true;
  await elements.get('motorRight').onclick();
  assert.match(elements.get('motorMessage').textContent,/Nie potwierdzono/);
  assert.equal(elements.get('motorStop').disabled,false);
  assert.equal(elements.get('motorLeft').disabled,false);
  fail=false;
  await elements.get('servoCenter').onclick();
  assert.deepEqual(calls.at(-1),{path:'/api/servo',data:{action:'move',angle:'90',speed:'50'}});
  assert.equal(elements.get('motorLeft').disabled,true);
  assert.equal(elements.get('servoRelease').disabled,false);
  await elements.get('servoRelease').onclick();
  assert.deepEqual(calls.at(-1).data,{action:'release'});
  assert.equal(elements.get('motorLeft').disabled,false);
  await elements.get('servoZero').onclick();
  assert.deepEqual(calls.at(-1).data,{action:'zero'});
  assert.match(elements.get('servoPosition').textContent,/Zapisane zero .*90/);
  await elements.get('servoHome').onclick();
  assert.deepEqual(calls.at(-1).data,{action:'home',speed:'50'});
  await elements.get('servoRelease').onclick();
  await elements.get('servoPlus').onclick();
  assert.deepEqual(calls.at(-1).data,{action:'move',angle:'100',speed:'50'});
  await elements.get('servoRelease').onclick();
  await elements.get('servoProbeCenter').onclick();
  assert.deepEqual(calls.at(-1).data,{action:'pulse',us:'1500'});
  await elements.get('servoRelease').onclick();
  await elements.get('servoProbeMinus').onclick();
  assert.deepEqual(calls.at(-1).data,{action:'pulse',us:'1475'});
  await elements.get('servoRelease').onclick();
  elements.get('secondColor').value='#00ff00';elements.get('secondBrightness').value='35';
  await elements.get('secondManual').onsubmit({preventDefault(){}});
  assert.deepEqual(calls.at(-1),{path:'/api/second',data:{mode:'manual',color:'#00ff00',brightness:'35'}});
  assert.match(elements.get('secondState').textContent,/Własny kolor/);
  assert.equal(state.manualColor,'#0064ff');assert.equal(state.enabled,false);
  await elements.get('secondSync').onclick();assert.deepEqual(calls.at(-1).data,{mode:'sync'});
  await elements.get('secondOff').onclick();assert.deepEqual(calls.at(-1).data,{mode:'off'});
  assert.match(elements.get('secondState').textContent,/wyłączona/);
  fail=true;await elements.get('secondSync').onclick();
  assert.match(elements.get('secondMessage').textContent,/Nie potwierdzono/);
  assert.equal(elements.get('secondApply').disabled,false);
  fail=false;elements.get('secondGainR').value='100';elements.get('secondGainG').value='45';elements.get('secondGainB').value='65';
  await elements.get('secondBalance').onsubmit({preventDefault(){}});
  assert.deepEqual(calls.at(-1),{path:'/api/second-balance',data:{r:'100',g:'45',b:'65'}});
  await elements.get('secondBalanceReset').onclick();
  assert.deepEqual(calls.at(-1).data,{r:'100',g:'100',b:'100'});
  await elements.get('effectStart').onclick();
  assert.deepEqual(calls.at(-1).data,{effect:'1',brightness:'100'});
  assert.match(elements.get('effectState').textContent,/Świeczka/);
  await elements.get('effectStop').onclick();assert.equal(state.lightEffect,0);
  for(const [effect,name] of [[6,'Zorza'],[7,'Ocean'],[8,'Zachód słońca'],[9,'Bicie serca'],[10,'Ping-pong']]){
    elements.get('lightEffectSelect').value=String(effect);
    await elements.get('effectStart').onclick();
    assert.deepEqual(calls.at(-1),{path:'/api/light-effect',data:{effect:String(effect),brightness:'100'}});
    assert.match(elements.get('effectState').textContent,new RegExp(name));
  }
  // Test usuwa aktywny efekt i korekcję tłumiącą czerwień, synchronizuje kontrolki.
  state={...state,secondGainR:0,brightness:20,manualSaturation:10};
  await elements.get('secondTestMagenta').onclick();
  assert.deepEqual(calls.at(-1),{path:'/api/light-test',data:{color:'3'}});
  assert.equal(state.lightEffect,0);assert.equal(state.secondMode,'sync');
  assert.equal(elements.get('color').value,'#ff00ff');
  assert.equal(elements.get('saturation').value,100);assert.equal(elements.get('brightness').value,100);
  assert.equal(elements.get('secondGainR').value,100);
  assert.match(elements.get('secondDutyState').textContent,/RGB 1: 255 \/ 0 \/ 255 · RGB 2: 255 \/ 0 \/ 255/);
  for(const [name,color] of [['Red','0'],['Green','1'],['Blue','2'],['Pink','4']]){
    await elements.get('secondTest'+name).onclick();assert.deepEqual(calls.at(-1).data,{color});
  }
  fail=true;await elements.get('secondTestRed').onclick();
  assert.match(elements.get('secondMessage').textContent,/Nie potwierdzono testu/);
  assert.equal(elements.get('secondTestRed').disabled,false);fail=false;

  await elements.get('motorAnimStart').onclick();
  assert.deepEqual(calls.at(-1).data,{device:'motor',effect:'swing'});
  assert.equal(elements.get('servoMove').disabled,true);
  assert.equal(elements.get('motorLeft').disabled,true);
  assert.equal(elements.get('motorAnimStop').disabled,false);
  await elements.get('motorAnimStop').onclick();assert.equal(state.motionEffect,0);
  await elements.get('servoAnimStart').onclick();assert.deepEqual(calls.at(-1).data,{device:'servo',effect:'wave'});
  assert.equal(elements.get('motorAnimStart').disabled,true);
  await elements.get('servoAnimStop').onclick();assert.equal(state.motionEffect,0);
  await elements.get('motorAnimLoop').onclick();
  assert.deepEqual(calls.at(-1).data,{device:'motor',effect:'swing',loop:'1'});
  assert.match(elements.get('motorAnimState').textContent,/Pętla aktywna/);
  assert.equal(elements.get('servoAnimLoop').disabled,true);
  await elements.get('motorAnimStop').onclick();
  await elements.get('servoAnimLoop').onclick();
  assert.deepEqual(calls.at(-1).data,{device:'servo',effect:'wave',loop:'1'});
  assert.equal(elements.get('motorAnimLoop').disabled,true);
  await elements.get('servoAnimStop').onclick();
  console.log('UI controls, defaults and effects: PASS');
})().catch(error=>{console.error(error);process.exitCode=1});
