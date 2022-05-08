// courtesy: Scott Pruett

/*
 * The purpose of this file is to separate the specific 'fetch' library -
 * here, superagent, from the rest of the application.
 */
import { sprintf } from 'sprintf-js';
import request from 'superagent';
// Side note:
// import { apiprefix } from '../config/';
// does not work because 'apiprefix' is not a named export of config!

import config from '../config/';

export function buildURL(/* template, printf arguments... */) {
  const url = sprintf.apply(null, arguments);
  return {
    builtURL: config.apiPrefix + url,
  };
}

export class APIResource {
  constructor(url) {
    //invariant(url != null, 'No URL given for APIResource');
    //invariant(url.builtURL != null, 'URLs must be made with buildURL');
    this.url = url.builtURL;
  }

  get(params) {
    // as per https://visionmedia.github.io/superagent/#setting-accept
    return request.get(this.url).accept('json').query(params);
  }
  put(data) {
    return request.put(this.url).send(data);
  }
  post(data) {
    return request.post(this.url).send(data);
  }
  delete() {
    return request.delete(this.url);
  }
}
