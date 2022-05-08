// Courtesy: Scott Pruett
//
// Returns a reducer that can process the following actions on baseType:
//  :BEGIN  - object is now loading
//  :OK     - object has loaded, apply transformResponse if any
//  :ERROR  - an error was encountering during loading
//
// The reduced state includes a 'loadingStatus' property: { loading, error, ok }
//
import { errorObject, loadedObject, loadingObject } from '../util/loadingObject';

export default function asyncHandler(baseType, initialState, transformResponse) {
  return (state, action) => {
    if (state == null || action.type === baseType + ':RESET') {
      if (initialState == null) {
        return loadingObject();
      }
      return initialState;
    }
    if (action.type === baseType + ':BEGIN') {
      return loadingObject(state);
    } else if (action.type === baseType + ':OK') {
      let response = action.response;
      if (transformResponse != null) {
        response = transformResponse(response);
      }
      return loadedObject(response);
    } else if (action.type === baseType + ':ERROR') {
      return errorObject({ ...state, error: action.error });
    }

    return state;
  };
}
