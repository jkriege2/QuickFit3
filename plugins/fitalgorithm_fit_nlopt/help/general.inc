<h2>General Description of Algorithms from NLopt</h2>
<h3>Use optimization algorithms for model fitting</h3>
<p>The non-linear optimization algorithms in NLopt solve the following very general optimization problem:
$$bmath:\vec{p}^\ast=\argmin_{\vec{p}}f(\vec{p}),$$
where - for the sake of fitting - we set the objective function $$math:f(\vec{p})$$ to a general least-squares objective function:
$$bmath:f(\vec{p})=\sum_{i=1}^N\left(\frac{y_i-m(x_i, \vec{p})}{\sigma_i}\right)^2,$$
where $(N)$ is the number of data points $((x_i,y_i))$ in a fitting problem. The model function is $(m(x,\vec{p}))$ and $(\sigma_i)$ are used to weight the different measurements, e.g. by error estimates for the different measurements.
</p>
<h3>Local optimization algorithm</h3>
<p>Some of the optimization algorithms need a local optimization method. Usually the Low-storage BFGS algorithm is used $$ref:Nocedal1980:J. Nocedal, "Updating quasi-Newton matrices with limited storage," <i>Math. Comput.</i> <b>35</b>, 773-782 (1980).$$, which uses numerical derivatives, described below..</p>
<h3>Numerical derivatives</h3>
<p>Some optimization algorithms in NLopt need an estimate of the derivatives $(\left.\partial f/\partial p_i\right|_{\vec{p}})$ of the objective function. The least-squares objectve function, given above has the following derivative
$[\left.\frac{\partial f}{\partial p_i}\right|_{\vec{p}} = \sum_{i=1}^N2\cdot\left(\frac{y_i-m(x_i, \vec{p})}{\sigma_i}\right)\cdot\left.\frac{\partial m}{\partial p_i}\right|_{x_i,\vec{p}}]$
where the model function derivatives are estimated using the 5-point stencil rule: $[\frac{\partial m(p)}{\partial p}\approx\frac{-m(p+2h)+8m(p+h)-8m(p-h)+m(p-2h)}{12h}.]$
</p>