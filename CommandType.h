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

    // ---- احراز هویت تکمیلی ----
    ForgotPasswordRequest,   // ارسال سوال امنیتی برای بازیابی
    ResetPassword,           // تعیین رمز جدید با پاسخ صحیح
    ChangePassword,          // تغییر رمز توسط کاربر لاگین‌شده

    // ---- پروفایل و ژانر ----
    GetProfile,
    UpdateProfile,
    SetFavoriteGenres,

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

    // ---- پاسخ‌های عمومی سرور ----
    ResponseOk,
    ResponseError,

    Unknown
};