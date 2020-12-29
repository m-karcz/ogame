import React from 'react';
import ReactDOM from 'react-dom';
import './index.css';
import App from './App';
import reportWebVitals from './reportWebVitals';
import {Provider} from 'react-redux'
import { createStore } from 'redux';


const initialState = {
    "loginRequested" : null
}

function myReducer(state = initialState, action: any)
{
  console.log("reducer kicks in");
  switch(action.type)
  {
    case "LOGIN_SUBMIT":
    {
        return {
          ...state,
          "loginRequested" : action.payload
        }
    }
    case "REGISTER_SUBMIT":
    {
        return {
          ...state,
          "registerRequested": action.payload
        }
    }
    case "REGISTER_SUCCESSFUL":
    {
        return {
          ...state,
          "registerRequested": null
        }
    }
    default:
    {
      return state;
    }
  }
}

const store = createStore(myReducer)


ReactDOM.render(
  <React.StrictMode>
    <Provider store = {store}>
      <App />
    </Provider>
  </React.StrictMode>,
  document.getElementById('root')
);

// If you want to start measuring performance in your app, pass a function
// to log results (for example: reportWebVitals(console.log))
// or send to an analytics endpoint. Learn more: https://bit.ly/CRA-vitals
reportWebVitals();
