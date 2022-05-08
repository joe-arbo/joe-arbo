import React from 'react';
import { connect } from 'react-redux';
import TopNavBar from '../components/TopNavBar';
import Logout from '../components/Logout';
import LoginPage from '../pages/LoginPage';
import PublicPage from '../pages/PublicPage';
import NotFoundPage from '../pages/NotFoundPage';
import HomePage from '../pages/HomePage';
import PrivatePage from '../pages/PrivatePage';
import PlayerPage from '../pages/PlayerPage';

import { Switch, Route, withRouter } from 'react-router-dom';

import config from '../config/';

/** AppContainer renders the navigation bar on top and its
 * children in the main part of the page.  Its children will
 * be chosen based on the selected route.
 */
const AppContainer = (props) => (
    <div>
      <TopNavBar branding="CS3214 Demo App 2021"
                  menus={config.menus} 
                  user={props.user} 
                  loginUrl={`/login`}
                  logoutUrl={`/logout`}
        />
      <div className="container-fluid marketing">
          <Switch>
              <Route exact path={`/`} component={HomePage} />
              <Route path={`/logout`} component={Logout} />
              <Route path={`/login`} component={LoginPage} />
              <Route path={`/public`} component={PublicPage} />
              <Route path={`/protected`} component={PrivatePage} />
              <Route path={`/player`} component={PlayerPage} />
              <Route component={NotFoundPage} />
          </Switch>
      </div>
    </div>
);

function mapStateToProps(state) {
  return {
    user: state.auth
  };
}

function mapDispatchToProps(dispatch) {
  return {
    dispatch
  };
}

export default withRouter(connect(mapStateToProps, mapDispatchToProps)(AppContainer));
