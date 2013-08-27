<p>This derivative-free optimization algorithm was introduced by Nelder and Mead$$ref:NelderMead:Nelder, John Ashworth; Mead, Roger: A Simplex Method for Function Minimization. Computer Journal, 1965, 7, 308-313 (<a href="http://comjnl.oxfordjournals.org/content/7/4/308.full.pdf">PDF</a>)$$  
It builds a simplex (the simplest object with non-zero volume in a D-dimensional space, e.g. a triangle in 2D, a tetrahedron in 3D, ...) in the D-dimensional search space (D=number of parameters), i.e. an object with D+1 points $$math:\vec{p}_i, 1\leq i\leq D+1$$, each representing a set of test-parameters of the 
function $$bmath:\chi^2(\vec{p})=\sum_{n=1}^N\left(\frac{f(x_n;\vec{p})-y_n}{\sigma_i}\right)^2$$ to be optimized. here $$math:f(x_n;\vec{p})$$ is the fit function to be fitted against the measured dataset $$math:(x_n,y_n)_{n=1..N}$$ and $$\sigma_n$$ are the weights for this dataset.</p>
<p>
Then the simplex is transformed in a certain way (basically the worst estiate is replaced by a new guess), until either the maximum number of iterations is reached, or no more improvement is possible.
</p>

