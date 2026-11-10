#pragma once

#include "Protocol.hpp"

class Cal
{
public:
    Response Enque(Request &req)
    {
        Response res(0, 0);
        switch (req.GetOper())
        {
        case '+':
        {
            res.SetResult(req.GetX() + req.GetY());
        }
        break;
        case '-':
        {
            res.SetResult(req.GetX() - req.GetY());
        }
        break;
        case '*':
        {
            res.SetResult(req.GetX() * req.GetY());
        }
        break;
        case '/':
        {
            if (req.GetY() == 0)
                res.SetCode(1);
            res.SetResult(req.GetX() / req.GetY());
        }
        break;
        case '%':
        {
            if (req.GetY() == 0)
                res.SetCode(2);
            res.SetResult(req.GetX() % req.GetY());
        }
        break;
        default:
            res.SetCode(3); // 非法操作
            break;
        }

        return res;
    }
};