ScatterJS.plugins(new ScatterEOS());

const network = ScatterJS.Network.fromJson({
    blockchain:'eos',
    chainId:'8a34ec7df1b8cd06ff4a8abbaa7cc50300823350cadc59ab296cb00d104d2b8f',
    host:'localhost',
    port:8888,
    protocol:'http'
});

ScatterJS.connect('RoscaApp',{network}).then(connected => {
if(!connected) return console.error('no scatter');
ScatterJS.login().then(id => {
    if(!id) return console.error('no identity');
})
});

const setStatus = () =>{
    const status = document.getElementById('status');
    if(!ScatterJS) return status.innerText = 'No scatter';
    if(!ScatterJS.identity) return status.innerText = 'No identity';
    status.innerText = ScatterJS.identity.accounts[0].name;
}

window.register = () =>{
    const sendTx = async (from, to) =>{
        const rpc = new eosjs_jsonrpc.JsonRpc('http://127.0.0.1:8888');
        const api = ScatterJS.eos(network, eosjs_api.Api, {rpc});

        const tx = await api.transact({
            actions: [{
            account: "rosca.token",
            name: "transfer",
            authorization: [{
                actor: from,
                permission: "active",
            }],
            data: {
                from: from,
                to: to,
                quantity: "10.00 ROSCA",
                memo: ""
            }
        }]
        }, {
        blocksBehind: 3,
        expireSeconds: 30
        });
    }
    sendTx('alice')
// ScatterJS.login().then(id => {
//     if(!id) return console.error('no identity');
//     const account = ScatterJS.account('eos');
    
//     const rpc = new JsonRpc('http://127.0.0.1:8888');
//     const api = ScatterJS.eos(network, Api.default, {rpc});

//     api.transact({
//         actions: [{
//         account: "rosca.token",
//         name: "transfer",
//         authorization: [{
//             actor: account.name,
//             permission: "active",
//         }],
//         data: {
//             from: account.name,
//             to: 'bob',
//             quantity: "10.00 ROSCA",
//             memo: ""
//         }
//     }]
//     }, {
//     blocksBehind: 3,
//     expireSeconds: 30
//     }).then(res => {
//         console.log('sent tx: ', res);
//     }).catch(err => {
//         console.error('error thrown: ', err);
//     });
// })
}