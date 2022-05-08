import React from 'react';

/* Example of 'stateless function' syntax to define React components.
 * See https://facebook.github.io/react/docs/reusable-components.html#stateless-functions
 */
const NotFound = (props) => 
  (<div><h1>Route not found Error</h1>
    <div>
        Something went wrong: {window.location.pathname} could not be found!
    </div>
        {/* JSON.stringify(props) */}
  </div>);

export default NotFound

