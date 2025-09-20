add_executable(playg playg.c)
target_link_libraries(playg vct)

add_executable(itm itm.c)
target_link_libraries(itm vct)

add_executable(push_pop_get_iterate push_pop_get_iterate.c)
target_link_libraries(push_pop_get_iterate vct)

add_executable(freestanding freestanding.c)
