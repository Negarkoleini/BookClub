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
    GetSuggestedBooks,   // بر اساسِ favoriteGenres کاربر
    GetPopularBooks,     // بر اساسِ میانگینِ امتیاز
    GetBestsellingBooks, // بر اساسِ تعدادِ فروش
    GetFreeBooks,
    GetNewestBooks,
    GetUserPurchaseHistory,

    // ---- احراز هویت تکمیلی ----
    ForgotPasswordRequest,   // ارسال سوال امنیتی برای بازیابی
    ResetPassword,           // تعیین رمز جدید با پاسخ صحیح
    ChangePassword,          // تغییر رمز توسط کاربر لاگین‌شده

    // ---- پروفایل و ژانر ----
    GetProfile,
    UpdateProfile,
    SetFavoriteGenres,
    DepositMoney,

    // ---- سبد خرید ----
    AddToCart,
    RemoveFromCart,
    GetCart,
    Checkout,

    // ---- کتابخانه شخصی ----
    SaveBookForLater,
    RemoveSavedBook,
    GetLibrary,
    CreateShelf,
    DeleteShelf,
    AddBookToShelf,
    RemoveBookFromShelf,
    SavePageLocation,
    GetPageLocation,

    // ---- نظرات/امتیاز  ----
    EditComment,
    DeleteComment,
    GetCommentsForBook,

    // ---- ناشر ----
    ApplyDiscount,
    RemoveDiscount,
    GetPublisherAnalytics,

    // ---- ادمین: کاربران ----
    GetAllUsers,
    BanUser,
    UnbanUser,
    DisableUserTemporarily,
    ChangeUserRole,
    DeleteUserAccount,
    GetUserDetails,

    // ---- ادمین: نظارت بر محتوا ----
    GetPendingComments,
    ApproveComment,
    RejectComment,
    GetAllComments,

    GetAllBooksAdmin,

    GetPendingDiscounts,
    ApproveDiscount,
    RejectDiscount,

    // ---- ادمین: محدودیت‌های سیستمی ----
    GetLimits,
    SetLimits,

    // ---مطالعه گروهی ----
    JoinReadingSession,
    LeaveReadingSession,
    SyncReadingPage,

    // ---- اعلان‌ها  ----
    GetNotifications,
    MarkNotificationRead,

    // ---- قفسه‌های شخصی (تکمیلی) ----
    GetShelves,
    RenameShelf,
    MoveBookBetweenShelves,

    // ---- پاسخ‌های عمومی سرور ----
    ResponseOk,
    ResponseError,

    Unknown
};

#include <QString>

// نام قابل‌خواندن هر دستور، برای نمایش در لاگ درخواست‌ها/پاسخ‌های داشبورد سرور
inline QString commandTypeToString(CommandType type)
{
    switch (type) {
#define BC_CASE(x) case CommandType::x: return QStringLiteral(#x)
    BC_CASE(Login); BC_CASE(Register); BC_CASE(Logout);
    BC_CASE(AddBook); BC_CASE(EditBook); BC_CASE(DeleteBook);
    BC_CASE(BuyBook);
    BC_CASE(AddComment); BC_CASE(AddRating);
    BC_CASE(Notification);
    BC_CASE(GetBooks); BC_CASE(GetBookDetails); BC_CASE(GetSuggestedBooks);
    BC_CASE(GetPopularBooks); BC_CASE(GetBestsellingBooks); BC_CASE(GetFreeBooks);
    BC_CASE(GetNewestBooks); BC_CASE(GetUserPurchaseHistory);
    BC_CASE(ForgotPasswordRequest); BC_CASE(ResetPassword); BC_CASE(ChangePassword);
    BC_CASE(GetProfile); BC_CASE(UpdateProfile); BC_CASE(SetFavoriteGenres); BC_CASE(DepositMoney);
    BC_CASE(AddToCart); BC_CASE(RemoveFromCart); BC_CASE(GetCart); BC_CASE(Checkout);
    BC_CASE(SaveBookForLater); BC_CASE(RemoveSavedBook); BC_CASE(GetLibrary);
    BC_CASE(CreateShelf); BC_CASE(DeleteShelf); BC_CASE(AddBookToShelf);
    BC_CASE(RemoveBookFromShelf); BC_CASE(SavePageLocation); BC_CASE(GetPageLocation);
    BC_CASE(EditComment); BC_CASE(DeleteComment); BC_CASE(GetCommentsForBook);
    BC_CASE(ApplyDiscount); BC_CASE(RemoveDiscount); BC_CASE(GetPublisherAnalytics);
    BC_CASE(GetAllUsers); BC_CASE(BanUser); BC_CASE(UnbanUser);
    BC_CASE(DisableUserTemporarily); BC_CASE(ChangeUserRole); BC_CASE(DeleteUserAccount);
    BC_CASE(GetUserDetails);
    BC_CASE(GetPendingComments); BC_CASE(ApproveComment); BC_CASE(RejectComment); BC_CASE(GetAllComments);
    BC_CASE(GetAllBooksAdmin);
    BC_CASE(GetPendingDiscounts); BC_CASE(ApproveDiscount); BC_CASE(RejectDiscount);
    BC_CASE(GetLimits); BC_CASE(SetLimits);
    BC_CASE(JoinReadingSession); BC_CASE(LeaveReadingSession); BC_CASE(SyncReadingPage);
    BC_CASE(GetNotifications); BC_CASE(MarkNotificationRead);
    BC_CASE(GetShelves); BC_CASE(RenameShelf); BC_CASE(MoveBookBetweenShelves);
    BC_CASE(ResponseOk); BC_CASE(ResponseError);
    BC_CASE(Unknown);
#undef BC_CASE
    }
    return QStringLiteral("Unknown");
}