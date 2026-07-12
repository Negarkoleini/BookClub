#pragma once

enum class CommandType
{
    Login,
    Register,
    Logout,

    AddBook,
    EditBook,
    DeleteBook,

    BuyBook,

    AddComment,
    AddRating,

    Notification,

    GetBooks,
    GetBookDetails,
    ResponseOk,
    ResponseError,
    ForgotPasswordRequest,
    ResetPassword,
    ChangePassword,
    ApproveComment,
    GetPendingComments,
    ApplyDiscount,
    GetPublisherAnalytics,

    GetAllUsers,
    BanUser,
    UnbanUser,
    DisableUserTemporarily,

    Unknown
};