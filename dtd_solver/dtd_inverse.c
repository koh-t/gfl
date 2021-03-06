// Invert a subset of a symmetric tridiagonal matrix in O(k^3) time.

#include <matrix.h>
#include <mex.h>

#define CHECK(X)                                \
  if (!(X))                                     \
    mexErrMsgIdAndTxt("invert_tridiag:check", "check failed: " #X)

enum InputArguments {
  INPUT_A,  // diagonal
  INPUT_IDX,
  NUM_INPUT_ARGS
};

double* getVector(const mxArray* array, int* size) {
  if (mxGetM(array) == 1) {
    *size = mxGetN(array);
  } else if (mxGetN(array) == 1) {
    *size = mxGetM(array);
  } else {
    mexErrMsgIdAndTxt("invert_trdiag:arguments", "input is not a vector.");
  }
  return mxGetPr(array);
}

void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) {
  if (nrhs != NUM_INPUT_ARGS) {
    mexErrMsgIdAndTxt("invert_trdiag:arguments",
                      "Wrong number of input arguments.");
  }

  int n, k;
  double* a = getVector(prhs[INPUT_A], &n);
  double* idx = getVector(prhs[INPUT_IDX], &k);
  double* theta = mxCalloc(n+1, sizeof(double));
  double* phi = mxCalloc(n+1, sizeof(double));

  theta[0] = 1;
  theta[1] = (2.0 + a[0]);
  for (int i = 1; i < n; i++)
    theta[i+1] = (2.0 + a[i])*theta[i] - theta[i-1];

  phi[n] = 1;
  phi[n-1] = (2.0 + a[n-1]);
  for (int i = n-2; i >= 0; i--)
    phi[i] = (2.0 + a[i])*phi[i+1] - phi[i+2];

  plhs[0] = mxCreateDoubleMatrix(k, k, mxREAL);
  double* B = mxGetPr(plhs[0]);

  for (int ki = 0; ki < k; ki++) {
    int i = (int)idx[ki]-1;
    CHECK(i >= 0 && i < n);

    for (int kj = ki; kj < k; kj++) {
      int j = (int)idx[kj]-1;
      CHECK(j >= 0 && j < n);
      B[ki*k+kj] = B[kj*k+ki] = theta[i]*phi[j+1]/theta[n];
    }
  }

  mxFree(theta);
  mxFree(phi);
}


