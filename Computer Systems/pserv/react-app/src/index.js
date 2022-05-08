import React from 'react';
import ReactDOM from 'react-dom';
import { Provider } from 'react-redux';
import { BrowserRouter as Router } from 'react-router-dom';

import 'bootstrap/dist/css/bootstrap.css'

import './style/logo.css';
import toastr from 'toastr';
import 'toastr/build/toastr.min.css';
import jQuery from 'jquery';    // for toastr;

import AppContainer from './containers/AppContainer';
import store from './store';
import {checklogin} from './actions/auth';
import config from './config';

toastr.options.closeButton = true;
toastr.options.positionClass = 'toast-bottom-right';

// request /api/login with current cookie (if any) to see if we're logged in. 
store.dispatch(checklogin());

const mountPoint = document.getElementById('root');
const rootNode = (
  <Provider store={store}>
    <Router basename={config.publicUrl}>
      <AppContainer />
    </Router>
  </Provider>
);
ReactDOM.render(rootNode, mountPoint);

window.jQuery = jQuery; // for toastr
