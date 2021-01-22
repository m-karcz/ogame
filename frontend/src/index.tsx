import React from 'react';
import ReactDOM from 'react-dom';
import './index.css';
import Login from './Login';
import reportWebVitals from './reportWebVitals';
import {Provider} from 'react-redux'
import { createStore, applyMiddleware} from 'redux';
import {
  BrowserRouter as Router,
  Switch,
  Route
} from "react-router-dom";
import {combinedReducer} from "./Reducers"
import {DEFAULT_STORE_STATE} from "./Store"
import Ingame from "./Ingame"
import Page from "./Page"
import RouterConnectivity from "./RouterConnectivity"
import { composeWithDevTools } from 'redux-devtools-extension';
import { getIngameMiddleware } from "./middlewares/InGamePageProcedures"
import { getLoginMiddleware } from "./middlewares/LoginPageProcedures"

const conn = new RouterConnectivity();


const store = createStore(combinedReducer, DEFAULT_STORE_STATE, composeWithDevTools(applyMiddleware(getIngameMiddleware(conn), getLoginMiddleware(conn))));

ReactDOM.render(

  <React.StrictMode>
    <Provider store = {store}>
      <div>elo</div>
      <Page/>
    </Provider>
  </React.StrictMode>,
  document.getElementById('root')
);

// If you want to start measuring performance in your app, pass a function
// to log results (for example: reportWebVitals(console.log))
// or send to an analytics endpoint. Learn more: https://bit.ly/CRA-vitals
reportWebVitals();
