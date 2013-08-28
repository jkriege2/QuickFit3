<h2>Implementation of Box constraints</h2>
As the underlying lmfit library does not support box constraints, they are implemented in this plugin by transforming the parameters using a tanh() function.
<center>
	<a name="fit_lmfit_pic000"><img src="./pic/fit_lmfit_pic000.png" border="1"><br/>
<i>tanh() function</i>
</center>
Each parameter $$math:s_i\leq p_i\leq e_i$$ is transformed into a parameter in the range $$math:-\infty<p_i'<\infty$$, by:
$$bmath:p_i'=\tanh^{-1}\left(2\cdot\frac{p_i-s_i}{e_i-s_i}-1.0\right)$$
Then the fit is performed on these paramaters and finally the transformation is reversed.
