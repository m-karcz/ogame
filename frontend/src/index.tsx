import React from 'react';
import ReactDOM from 'react-dom';
import './index.css';
import Login from './Login';
import reportWebVitals from './reportWebVitals';
import {Provider} from 'react-redux'
import { createStore, combineReducers, AnyAction, Reducer} from 'redux';
import {
  BrowserRouter as Router,
  Switch,
  Route,
  Link,
  useHistory
} from "react-router-dom";
import {combinedReducer} from "./Reducers"
import {DEFAULT_STORE_STATE} from "./Store"
import Ingame from "./Ingame"

const store = createStore(combinedReducer, DEFAULT_STORE_STATE);

ReactDOM.render(

  <React.StrictMode>
    <Provider store = {store}>
      <div>elo</div>
      <Router>
        <Switch>
          <Route exact path="/">
            <Login />
          </Route>
          <Route path="/ingame">
            <div>ingame</div>
            <Ingame />
          </Route>
        </Switch>
      </Router>
    </Provider>
  </React.StrictMode>,
  document.getElementById('root')
);

// If you want to start measuring performance in your app, pass a function
// to log results (for example: reportWebVitals(console.log))
// or send to an analytics endpoint. Learn more: https://bit.ly/CRA-vitals
reportWebVitals();
