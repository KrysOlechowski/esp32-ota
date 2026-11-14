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
elements.get('color').value = '#0064ff';
elements.get('brightness').value = '60';
elements.get('saturation').value = '100';
let state = {device:'test',temperature:20,humidity:50,sensorOK:true,pwmOK:true,enabled:false,
  mode:'manual',ageSeconds:0,color:'#000000',manualColor:'#0064ff',brightness:60,
  manualSaturation:100,cold:17,warm:22,dry:30,wet:70,motorRunning:false,motorDirection:0,servoActive:false,servoOK:true,servoTarget:90};
let calls = [], fail = false;
const context = vm.createContext({URLSearchParams,document:{getElementById:id=>elements.get(id),
  querySelectorAll:selector=>[...elements].filter(([id])=>selector.includes(':not')&&!id.startsWith('motor')).map(([,el])=>el)},
  setTimeout(){},fetch:async(path,options)=>{
    if(options?.body){
      const data=Object.fromEntries(options.body);
      calls.push({path,data});
      if(fail)throw Error('offline');
      if(path==='/api/servo')state={...state,servoActive:['move','home'].includes(data.action),servoHasCommand:true,servoZeroKnown:state.servoZeroKnown||data.action==='zero',servoZero:data.action==='zero'?state.servoTarget:state.servoZero,servoTarget:Number(data.angle??state.servoTarget)};
      if(path==='/api/motor')state={...state,motorRunning:!['stop','zero','home'].includes(data.action),motorDirection:data.action==='stop'?0:data.action==='left'?-1:1,motorDiagnostic:data.action==='test'};
    }
    return {ok:true,json:async()=>({...state})};
  }});
(async()=>{
  vm.runInContext(script,context);
  await new Promise(resolve=>setImmediate(resolve));
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
  assert.match(elements.get('servoPosition').textContent,/Zapisane zero: 90/);
  await elements.get('servoHome').onclick();
  assert.deepEqual(calls.at(-1).data,{action:'home',speed:'50'});
  await elements.get('servoRelease').onclick();
  await elements.get('servoPlus').onclick();
  assert.deepEqual(calls.at(-1).data,{action:'move',angle:'100',speed:'50'});
  console.log('Stepper and servo web controls: PASS');
})().catch(error=>{console.error(error);process.exitCode=1});
