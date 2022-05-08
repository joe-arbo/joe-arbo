import React from 'react';
import { Container, Row, Label, Input, Alert, Button, ButtonToolbar, Form, FormGroup } from 'reactstrap';
import { withFormik } from 'formik';

/*
 * This is a form that display username/password.
 * When the user hits submit, the 'onSubmit' method is called
 * in the parent, which receives the username/password the user
 * entered.  It also performs validation.
 */
const LoginForm = (props) => {
  const {
    handleSubmit, // rest is from HOC
    isSubmitting,
    handleReset,
    handleBlur,
    handleChange,
    // errors,
    dirty,
    // touched,
    values,
    // valid
  } = props;

  return (
    <Form onSubmit={handleSubmit}>
      <Container>
        <Row>
          <FormGroup>
            <Label for="username">User Name</Label>
            <Input type="text" name="username" value={values.username}
              onChange={handleChange}
              onBlur={handleBlur}
            />
          </FormGroup>
          <FormGroup>
            <Label for="password">Password</Label>
            <Input type="password" name="password"
              onChange={handleChange}
              onBlur={handleBlur}
            />
          </FormGroup>

        { props.autherror && 
            <FormGroup>
              <Alert bsstyle='danger'>
                {props.autherror.message || 'Login failed'}
              </Alert>
            </FormGroup>
        }
        </Row>

        <Row>
            <ButtonToolbar>
              <Button
                type='submit'
                bsstyle='success' className="mr-2">
                Submit
              </Button> 
              <Button
                type="button"
                onClick={handleReset}
                disabled={!dirty || isSubmitting}>
                Reset
              </Button>
            </ButtonToolbar>
        </Row>
      </Container>
    </Form>
  );
};

export default withFormik({
  mapPropsToValues: () => ({ username: '', password: '' }),
  handleSubmit: (values, { setSubmitting, props }) => {
    props.onSubmit(values);
    setSubmitting(false);
  },
  displayName: 'LoginForm', // helps with React DevTools
})(LoginForm);

