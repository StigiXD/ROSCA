import React, { Component } from 'react'
import PropTypes from 'prop-types'

import { JsonRpc } from 'eosjs'
import { Anchor } from 'ual-anchor'
import { Scatter } from 'ual-scatter'
import { UALProvider, withUAL } from 'ual-reactjs-renderer'

const chains = [{
  chainId:'8a34ec7df1b8cd06ff4a8abbaa7cc50300823350cadc59ab296cb00d104d2b8f',
  name: 'ROSCA Testnet',
	rpcEndpoints:[{
		protocol:'http',
		host:'localhost',
		port:8888
	}]
}];

const getActionData = (actor, permission, chainId) => {
  // default data
  let data = {
    from: actor,
    to: 'bob',
    quantity: getTransactionAmount(chainId),
    memo: 'ual-anchor-demo'
  }
  return data
}

const getContractData = (chainId) => {
  // default contract/action
  let account = 'rosca.token'
  let name = 'transfer'

  return { account, name }
}

const getActions = (actor, permission, chainId) => {
  const { account, name } = getContractData(chainId)
  const data = getActionData(actor, permission, chainId)
  return {
    actions: [
      {
        account,
        name,
        authorization: [{ actor, permission }],
        data,
      }
    ],
  }
}

// const getTransaction = async (activeUser, actor, permission, chainId) => {
//   const { account, name } = getContractData(chainId)
//   const data = getActionData(actor, permission, chainId)
//   const actions = [{
//     account,
//     name,
//     authorization: [{ actor, permission }],
//     data,
//   }]
//   const info = await activeUser.rpc.get_info()
//   const height = info.last_irreversible_block_num - 3
//   const blockInfo = await activeUser.rpc.get_block(height)
//   const timePlus = new Date().getTime() + (60 * 1000)
//   const timeInISOString = (new Date(timePlus)).toISOString()
//   const expiration = timeInISOString.substr(0, timeInISOString.length - 1)
//   return {
//     actions,
//     context_free_actions: [],
//     transaction_extensions: [],
//     expiration,
//     ref_block_num: blockInfo.block_num & 0xffff,
//     ref_block_prefix: blockInfo.ref_block_prefix,
//     max_cpu_usage_ms: 0,
//     max_net_usage_words: 0,
//     delay_sec: 0,
//   }
// }

const getTransactionAmount = (chainId) => {
  let symbol = 'ROSCA'
  let quantity = '10.00'

  return `${quantity} ${symbol}`
}

const getCreateGroupAction = (actor, permission, contrib_amt, max_usrs) => {
  let account = 'creategroup'
  let name = 'create'
  let data = {
    user: actor,
    contrib_amt: contrib_amt,
    max_usrs: max_usrs
  }

  return {
    actions: [
      {
        account,
        name,
        authorization: [{ actor, permission }],
        data,
      }
    ],
  }
}

class TestApp extends Component {
  static propTypes = {
    ual: PropTypes.shape({
      activeUser: PropTypes.object,
      activeAuthenticator: PropTypes.object,
      logout: PropTypes.func,
      showModal: PropTypes.func,
    }).isRequired,
  }

  constructor(props) {
    super(props)
    this.state = {
      chainId: props.chainId,
      message: '',
    }
  }

  purchase = async () => {
    const { ual: { activeUser } } = this.props
    try {
      const { accountName, chainId } = activeUser
      let { requestPermission } = activeUser
      if (!requestPermission && activeUser.scatter) {
        requestPermission = activeUser.scatter.identity.accounts[0].authority
      }
      const demoActions = getActions(accountName, requestPermission, chainId)
      const result = await activeUser.signTransaction(demoActions, { expireSeconds: 120, blocksBehind: 3 })
      this.setState({
        message: `Transfer Successful!`,
      }, () => {
        setTimeout(this.resetMessage, 5000)
      })
      console.info('SUCCESS:', result)
    } catch (e) {
      console.error('ERROR:', e)
    }
  }

  createGroup = async () => {
    const { ual: { activeUser } } = this.props
    try {
      const { accountName} = activeUser
      let { requestPermission } = activeUser
      if (!requestPermission && activeUser.scatter) {
        requestPermission = activeUser.scatter.identity.accounts[0].authority
      }
      const demoAction = getCreateGroupAction(accountName, requestPermission, 10, 3)
      const result = await activeUser.signTransaction(demoAction, { expireSeconds: 120, blocksBehind: 3 })
      this.setState({
        message: `Transfer Successful!`,
      }, () => {
        setTimeout(this.resetMessage, 5000)
      })
      console.info('SUCCESS:', result)
    } catch (e) {
      console.error('ERROR:', e)
    }
  }

  resetMessage = () => this.setState({ message: '' })

  renderLoggedInView = () => (
    <>
      {!!this.state.message
        && (
          <div style={styles.announcementBar}>
            <p style={styles.baseText}>{this.state.message}</p>
          </div>
        )
      }
      <button type='button' onClick={this.purchase} style={{ ...styles.button, ...styles.blueBG }}>
        <p style={styles.baseText}>{`Transfer`}</p>
      </button>
      <table>
        <tbody>
          <tr>
            <td style={styles.td}>Blockchain</td>
            <td style={styles.td}>{this.props.ual.chains[0].name}</td>
          </tr>
          <tr>
            <td style={styles.td}>Account</td>
            <td style={styles.td}>{this.props.ual.activeUser.accountName}</td>
          </tr>
          <tr>
            <td style={styles.td}>Permission</td>
            <td style={styles.td}>{this.props.ual.activeUser.requestPermission}</td>
          </tr>
         
        </tbody>
      </table>
      <button type='button' onClick={this.props.ual.logout} style={styles.logout}>
        <p>Logout</p>
      </button>

    <button type='button' onClick={this.createGroup} style={styles.logout}>
      <p>Create Group</p>
    </button>
    </>
  )

  renderLoginButton = () => (
    <button type='button' onClick={this.props.ual.showModal} style={styles.button}>
      <p style={{buttonText: styles.buttonText, baseText: styles.baseText}}>Login using {this.props.ual.chains[0].name}</p>
    </button>
  )

  render() {
    const { ual: { activeUser } } = this.props
    return (activeUser) ? this.renderLoggedInView() : this.renderLoginButton()
  }
}

class UALWrapper extends Component {
  constructor(props) {
    super(props)
    const search = window.location.search
    const params = new URLSearchParams(search)
    const chainId = params.get('chain')
    this.state = {
      chainId: chainId || chains[0].chainId
    }
  }
 
  render () {
    const { chainId } = this.state
    const { available } = this.props
    const [ chain ] = available.filter((c) => c.chainId === chainId)
    if (!chain) {
      return (
        <div>Invalid Chain ID</div>
      )
    }
    const anchor = new Anchor([chain], {
      // Required: The name of the app requesting a session
      appName: 'ROSCA App',
      // Optional: define your own endpoint or eosjs JsonRpc client
      rpc: new JsonRpc('http://localhost:8888')
      // Optional: define API for session management, defaults to cb.anchor.link
      // service: 'https://cb.anchor.link'
    })
    const scatter = new Scatter([chain], { appName: 'ual-anchor-demo' });
    return (
      <div style={styles.container}>
        <UALProvider
          appName='Anchor + Authenticator Test App'
          authenticators={[anchor, scatter]}
          chains={[chain]}
          key={chain.chainId}
        >
          <TestAppConsumer />
        </UALProvider>
      </div>
    );
  }
}

const styles = {
  container: {
    display: 'flex',
    backgroundColor: '#fff',
    alignItems: 'center',
    justifyContent: 'top',
    height: '100vh',
    marginTop: '10vh',
    flexDirection: 'column',
  },
  button: {
    padding: '10px 60px',
    backgroundColor: '#EA2E2E',
    textAlign: 'center',
    borderRadius: 5,
    color: '#FFFFFF',
    fontSize: 18,
    fontWeight: 'bold',
    marginBottom: '2em',
  },
  select: {
    textAlign: 'center',
    borderRadius: 5,
    fontSize: 24,
    fontWeight: 'bold',
    width: '200px',
  },
  logout: {
    marginTop: 20,
  },
  baseText: {
    color: '#fff',
    fontSize: 18,
  },
  blueBG: {
    backgroundColor: '#447DD8',
  },
  announcementBar: {
    width: '100%',
    padding: '10px 50px 10px 20px',
    textAlign: 'center',
    backgroundColor: '#3de13d',
    top: 0,
    position: 'absolute',
    alignItems: 'center',
  },
}

const TestAppConsumer = withUAL(TestApp)

const App = () => (
  <UALWrapper available={chains} />
)

export default App
