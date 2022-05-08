import api from '../api';
import apiAction from './apiAction';
import toastr from 'toastr';

// this function is an "action creator"
// the action created, however, is a thunk.
// see apiAction.js
export function login(username, password) {
  return apiAction({
    baseType: 'LOGIN',
    fetch() {
      return api.auth.login({ username, password });
    },
    onSuccess(dispatch, data, getState) {
        toastr.success(`Success logging in: ${JSON.stringify(data)}`);
    },
    onError(dispatch, data, getState) {
        toastr.success(`Error logging in: ${JSON.stringify(data && data.response && data.response.text)}`);
    },
  });
}

export function checklogin() {
  return apiAction({
    baseType: 'CHECKLOGIN',
    fetch() {
      return api.auth.checklogin();
    },
    onSuccess(dispatch, data, getState) {
        toastr.success(`Server says ${JSON.stringify(data)}`);
    },
  });
}
