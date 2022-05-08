import { combineReducers } from 'redux';

import auth from './auth';
import video from './video';

const rootReducer = combineReducers({
  auth: auth,
  video: video
});

export default rootReducer;