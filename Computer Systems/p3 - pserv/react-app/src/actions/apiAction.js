/**
 * This is an action creator for actions that relate to accessing APIs.
 * Whereas normal action creators return a plain { type: ..., ... } action
 * object, this action creator returns a function.  This is possible
 * through the use of the redux-thunk middleware, see store.js
 * See https://github.com/gaearon/redux-thunk
 *
 * Courtesy: Scott Pruett
 */
export default function apiAction(
  { 
    baseType,   // the base type of the action, e.g. "LOGIN", etc.
    fetch,      // a promise-returning function
    onSuccess,  // a callback for when promise succeeded
    onError,    // a callback for when promise failed
    params      // params - passed backed to callbacks
  }
) {
  return (dispatch, getState) => {
    // dispatch *:BEGIN action to let everyone know that API request
    // has begun.  (Reducers can now set 'loadingStatus' to loading.)
    dispatch({
      type: baseType + ':BEGIN',
      params,
    });

    // initiate the API request
    fetch().then(
      response => {
        if (onSuccess) {
          onSuccess(dispatch, response.body, getState);
        }
        // on success, dispatch *:OK action with response
        dispatch({
          type: baseType + ':OK',
          response: response.body,
          params,
        });
      },
      error => {
        if (onError) {
          onError(dispatch, error, getState);
        }
        // on error, dispatch *:ERROR action with response
        dispatch({
          type: baseType + ':ERROR',
          error: error,
          params,
        });
      }
    );
  };
}
