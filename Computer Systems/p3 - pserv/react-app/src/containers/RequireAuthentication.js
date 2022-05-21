/*
 * This HOC can be used to wrap components so that if they are rendered without authentication,
 * they redirect to '/login' first and then come back.
 */

import React from 'react';
import { Redirect, withRouter } from 'react-router';
import { connect } from 'react-redux';
import { isLoaded } from '../util/loadingObject';

function mapStateToProps(state) {
  return {
    user: state.auth
  };
}

export default function RequireAuthentication(Component) {
  const wrapper = props => {
    if (isLoaded(props.user)) {
      return <Component {...props} />;
    } else {
      return (
        <Redirect
          to={{
            pathname: `/login`,
            state: {
              from: props.history.location
            }
          }}
        />
      );
    }
  };

  return withRouter(connect(mapStateToProps)(wrapper));
}
