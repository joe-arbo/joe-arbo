import api from '../api';
import apiAction from './apiAction';
import toastr from 'toastr';

// this function is an "action creator"
// the action created, however, is a thunk.
// see apiAction.js
export function list() {
  return apiAction({
    baseType: 'VIDEO',
    fetch() {
      return api.video.list();
    },
    onSuccess(dispatch, data, getState) {
      data.list = data;
      toastr.success(`Fetched video list with ${data.list.length} entries`);
    },
    onError(dispatch, data, getState) {
      toastr.success(`Error fetching video list: ${JSON.stringify(data && data.response && data.response.text)}`);
    },
  });
}