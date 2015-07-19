#pragma once

template<typename X, typename Y, typename D>
class InvertibleFunction_
{
    virtual Y operator()(X) = 0;
    virtual D derivativeAt(X) = 0;

public:
    X inverse(Y y, X guess, D eps)
    {
        D err = operator()(guess) - y;
        while (err > eps || err < -eps)
        {
            D derivative = derivativeAt(guess);

            // Avoid divide by 0
            if (derivative == (D)0)
                return guess;

            guess -= err / derivative;
            err = operator()(guess) - y;
        }

        return guess;
    }
};

typedef InvertibleFunction_<float, float, float> InvertibleFunction;
