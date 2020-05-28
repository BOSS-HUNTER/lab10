#include <boost/asio.hpp>

#include "LogSetup.h"
#include "Settings.h"
#include "Utils.h"
#include "DbActions.h"

int main(int argc, char *argv[])
{
    if (auto returnCode = programArguments(argc, argv); returnCode != 0) {
        return returnCode;
    }

    LogSetup::init();

    BOOST_LOG_TRIVIAL(debug) << "Log setup complete";
    BOOST_LOG_TRIVIAL(info) << "Input: " << Settings::input
                            << "\nOutput: " << Settings::output
                            << "\nThreads: " << Settings::threadAmount
                            << "\nLogLevel: " << Settings::logLevel;

    if (Settings::writeOnly) {
        BOOST_LOG_TRIVIAL(info) << "Creating random db...";

        DbActions actions{Settings::input};
        actions.create();
        actions.randomFill();

        return 0;
    }

    removeDirectoryIfExists(Settings::output);
    copyDirectory(Settings::input, Settings::output);

    DbActions actions{Settings::output};

    boost::asio::thread_pool pool(Settings::threadAmount);

    auto descriptors = actions.getFamilyDescriptorList();
    auto handlers = actions.open(descriptors);

    std::list<DbActions::RowContainer> cachedRows;
    for (auto &family: handlers) {
        cachedRows.push_back(
            actions.getRows(family.get())
        );
        auto &rows = cachedRows.back();

        size_t counter = 0;
        auto beginIterator = rows.cbegin();
        for (auto it = rows.cbegin(); it != rows.cend(); ++it) {
            counter++;

            static const size_t COUNTER_PER_THREAD = 4;
            if (counter != 0 && counter % COUNTER_PER_THREAD == 0) {
                boost::asio::post(pool,
                                  [&actions, &family, beginIterator, it]() {
                                      actions.hashRows(family.get(),
                                                       beginIterator,
                                                       it);
                                  });

                beginIterator = it;
            }
        }

        if (beginIterator != rows.cend()) {
            boost::asio::post(pool,
                              [&actions, &family, beginIterator, &rows]() {
                                  actions.hashRows(family.get(),
                                                   beginIterator,
                                                   rows.cend());
                              });
        }

    }

    pool.join();
}